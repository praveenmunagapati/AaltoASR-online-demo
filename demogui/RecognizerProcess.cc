
#include "RecognizerProcess.hh"
#include "str.hh"

const unsigned int RecognizerProcess::MIN_BEAM = 1;
const unsigned int RecognizerProcess::MAX_BEAM = 300;
const unsigned int RecognizerProcess::MIN_LMSCALE = 1;
const unsigned int RecognizerProcess::MAX_LMSCALE = 100;

RecognizerProcess::RecognizerProcess(const std::string &connect,
                                     const std::string &script,
                                     unsigned int beam,
                                     unsigned int lmscale) throw(Exception)
{
  this->m_connect = connect;
  this->m_script = script;
  
  // Try to read default settings from the script file.
  if (!this->set_beam(beam)) {
    throw Exception(str::fmt(100, "Beam value %u is not in range %u-%u",
                             beam, MIN_BEAM, MAX_BEAM));
  }

  if (!this->set_lmscale(lmscale)) {
    throw Exception(str::fmt(100, "LM-scale value %u is not in range %u-%u",
                             lmscale, MIN_LMSCALE, MAX_LMSCALE));
  }
}

void
RecognizerProcess::start()
{
  // Fork a child process.
  if (this->m_process.create() == 0) {
    // Child process enters here.
    // Parse the parameters to exec-function.
    std::vector<std::string> args = str::split(this->m_connect, " ", true);
    std::vector<std::string> scriptargs = str::split(this->m_script, " ", true);
    args.insert(args.end(), scriptargs.begin(), scriptargs.end());

    char **arg_array = new char*[args.size() + 1];

    // Copy the parameters into the array.      
    for (unsigned int i = 0; i < args.size(); i++)
      arg_array[i] = strdup(args[i].c_str());
    arg_array[args.size()] = NULL;
    
    // Execute the recognizer.
    int ret = execvp(arg_array[0], &arg_array[0]);
              
    if (ret < 0) {
      perror("Recognizer process failed exec()");
      // NOTE: Must use _exit NOT exit so Xlib will work properly!
      _exit(-1);                                    
    }
    // Child process never gets here.
    assert(false);
  }
  // Parent process continues here.
  
  msg::set_non_blocking(this->m_process.read_fd);
  msg::set_non_blocking(this->m_process.write_fd);
  
  this->m_in_queue.enable(this->m_process.read_fd);
  this->m_out_queue.enable(this->m_process.write_fd);

  // Synchronize settings.
  this->send_settings();
}

void
RecognizerProcess::finish()
{
  if (this->m_process.is_created()) {
    this->m_process.finish();
    this->m_in_queue.disable();
    this->m_out_queue.disable();
  }
}
/*
bool
RecognizerProcess::set_computer(const std::string &computer)
{
  if (!this->m_process.is_created()) {
    this->m_cluster = computer;
    return true;
  }
  return false;
}

bool
RecognizerProcess::set_script(const std::string &script)
{
  if (!this->m_process.is_created()) {
    this->m_script = script;
    return true;
  }
  return false;
}
//*/
bool
RecognizerProcess::set_beam(unsigned int beam)
//throw(msg::ExceptionBrokenPipe)
{
  if (beam < MIN_BEAM || beam > MAX_BEAM)
    return false;

  this->send_parameter("beam", beam);
  this->m_beam = beam;
  return true;
}

bool
RecognizerProcess::set_lmscale(unsigned int lmscale)
//throw(msg::ExceptionBrokenPipe)
{
  if (lmscale < MIN_LMSCALE || lmscale > MAX_LMSCALE)
    return false;

  this->send_parameter("lm_scale", lmscale);
  this->m_lmscale = lmscale;
  return true;
}

void
RecognizerProcess::send_settings()
{
  this->send_parameter("beam", this->m_beam);
  this->send_parameter("lm_scale", this->m_lmscale);
}

void
RecognizerProcess::send_parameter(const std::string &name, unsigned int value)
{
  msg::Message message(msg::M_DECODER_SETTING, true);
  char buffer[20];

  sprintf(buffer, "%s %d", name.data(), value);
  message.append(buffer);
  this->m_out_queue.add_message(message);
}
/*
bool
RecognizerProcess::read_settings()
{
  FILE *file;
  std::string line;
  std::vector<std::string> fields;
  bool read_settings = false;
  bool ok = true;
  unsigned int beam = 0, lmscale = 0;
  
  file = fopen(this->m_script.data(), "r");
  if (!file) {
    fprintf(stderr, "RecognizerProcess::read_settings: Error opening file.\n");
    return false;
  }

  while (!feof(file) && !read_settings) {
    str::read_line(&line, file, true);
    if (line.compare(0, 12, "./recognizer") == 0) {
      str::split(&line, " \"", true, &fields);
      for (unsigned int ind = 0; ind < fields.size() - 1; ind++) {
        if (fields[ind] == "--lm-scale") {
          lmscale = str::str2long(&fields[ind+1], &ok);
        }
        if (fields[ind] == "--beam") {
          beam = str::str2long(&fields[ind+1], &ok);
        }
      }
      read_settings = true;
    }
  }

  fclose(file);
  
  // Check validity.
  ok &= (beam >= MIN_BEAM && beam <= MAX_BEAM) &&
        (lmscale >= MIN_LMSCALE && lmscale <= MAX_LMSCALE);
        
  if (read_settings && ok) {
    this->m_beam = beam;
    this->m_lmscale = lmscale;
  }
  else {
    read_settings = false;
    fprintf(stderr,
            "Recognizer process failed to read settings from file \"%s\"\n",
            this->m_script.data());
  }
  
  return read_settings;
}
//*/
