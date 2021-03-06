#ifndef MESSENGER_HH
#define MESSENGER_HH

#include <cstdlib>
#include <sys/select.h>
#include <string>
#include <vector>
#include <deque>
#include <exception>
#include "endian.hh"

namespace msg {

  /* Message format:

     Bytes	Info
     4		message length in total (len)
     1		message type		
     1		urgent flag

     len - 6	data
  */

  /** Message types */
  enum { 
    M_AUDIO, 		// gui -> rec
    M_AUDIO_END,	// gui -> rec
    M_RESET, 		// gui -> rec

    M_PROBS, 		// rec <- ac, rec -> dec
    M_PROBS_END, 	// rec -> dec

    M_READY, 		// gui <- rec, rec <- dec, rec <- ac
    M_RECOG, 		// gui <- rec, rec <- dec
    M_RECOG_END,	// gui <- rec, rec <- dec

    // beam float
    // lm_stalce float
    M_DECODER_SETTING,	// gui -> rec -> dec
    M_DECODER_PAUSE,	// gui -> rec -> dec
    M_DECODER_UNPAUSE,	// gui -> rec -> dec

    M_ADAPT_ON,		// gui -> rec -> dec
    M_ADAPT_OFF,	// gui -> rec -> dec
    M_ADAPT_RESET,	// gui -> rec -> dec
    M_ADAPT_CALC,       // gui -> rec
    M_ADAPT_CANCELLED,  // gui <- rec
    M_STATE_HISTORY, 	// rec <- dec

    M_DEBUG,		// gui -> rec
    M_MESSAGE,		// gui <- rec <- dec
  };

  const int header_size = 6;

  void set_non_blocking(int fd);
  
  class ExceptionBrokenPipe  :  public std::exception
  {
  public:
    ExceptionBrokenPipe(int fd) throw() : m_fd(fd) { }
    inline int get_fd() const { return m_fd; }
    virtual const char* what() const throw () { return "Broken pipe."; }
  private:
    int m_fd;
  };

  class Message {
  public:
    std::string buf;
    bool raw;

    Message(int type = 0, bool urgent = false)
      : buf(header_size, 0), raw(false)
    {
      endian::put4(header_size, &buf[0]);
      buf.at(4) = type;
      buf.at(5) = urgent;
    }

    void set_type(int type)
    {
      buf.at(4) = type;
    }
    
    void set_urgent(bool urgent)
    {
      buf.at(5) = urgent;
    }
    
    void clear_data()
    {
      buf.erase(msg::header_size);
      endian::put4((int)buf.size(), &buf[0]);
    }

    void append(const std::string &str)
    {
      assert((int)buf.size() >= header_size);
      buf.append(str);
      endian::put4((int)buf.size(), &buf[0]);
    }
    
    // Jaakko: Lis�sin tarpeisiini t�llaisen funktion..
    void append(const char *str, unsigned int size)
    {
      assert((int)buf.size() >= header_size);
      buf.append(str, size);
      endian::put4((int)buf.size(), &buf[0]);
    }

    void append(float f)
    {
      assert((int)buf.size() >= header_size);
      std::string str(4, 0);
      endian::put4(f, &str[0]);
      buf.append(str);
    }

    int type() const
    {
      assert((int)buf.length() >= header_size);
      return buf[4];
    }

    bool urgent() const
    { 
      assert((int)buf.length() >= header_size);
      return buf[5] != 0;
    }

    int data_length() const
    {
      assert((int)buf.length() >= header_size);
      return buf.size() - header_size;
    }

    char *data_ptr()
    {
      return &buf[header_size];
    }

    const char *data_ptr() const
    {
      return &buf[header_size];
    }

    std::string data_str() const 
    {
      return std::string(data_ptr(), data_length());
    }
  };

  class InQueue {
  public:
    // Should this be protected/private?
    std::deque<Message> queue;
    std::string name; //!< Optional queue name for debugging purposes

  public:
    InQueue(int fd = -1);
    void enable(int fd);
    void disable();

    /** Suspend the queue so that mux ignores the queue. */
    void mux_suspend()
    {
      suspended = true;
    }

    /** Release the queue so that mux uses the queue again. */
    void mux_release()
    {
      suspended = false;
    }

    /** Fetch message from the file descriptor and insert it to the
     * queue.  For non-blocking files the function may return before
     * complete messages are read.
     */
    void flush();

    /** Check if queue is empty. */
    bool empty() { return queue.empty(); }

    /** File descriptor of the queue. */
    int get_fd() { return fd; }

    /** EOF reached? */
    bool get_eof() { return eof; }

    /** Return the suspend status of the queue. */
    bool is_suspended() { return suspended; }

  private:
    std::string buffer;
    int bytes_got;
    int fd;
    bool eof;
    bool suspended; //!< If true, mux ignores the queue
  };

  class OutQueue {
  public:
    // Should this be protected/private?
    std::deque<Message> queue;

  public:
    OutQueue(int fd = -1);
    inline bool empty() const { return queue.empty(); }

    /** Enable the queue. 
     * \param fd = file descriptor where messages are sent */
    void enable(int fd);

    /** Disable the queue, so that Mux won't watch the queue. */
    void disable();

    /** Move first message from the queue to internal send buffer */
    bool prepare_next();

    /** Send data from the internal send buffer. \note If the file
     * descriptor is in non-blocking state, some of the data may be unsent. 
     * \return false if whole message was not sent
     */
    bool send_next() throw(ExceptionBrokenPipe); 

    /** Flush all messages in the queue.  If the file descriptor is in
     * non-blocking state, the function returns if the file would block. */
    void flush() throw(ExceptionBrokenPipe);

    /** File descriptor of the queue. */
    inline int get_fd() const { return fd; }
    
    /** Puts the message in the queue. **/
    void add_message(const Message &msg);
    
    /** Removes all messages from the queue. */
    inline void clear() { this->queue.clear(); }
    
    /** Remove all non-urgent messages from the queue.
     * TODO: Better name for this function? */
    void clear_non_urgent();

  private:
    std::string buffer; //!< The internal buffer for the next message
    size_t bytes_sent; //!< Bytes sent from the internal buffer.
    int fd; //!< File descriptor where the messages are sent.
  };

  class Mux {
  public:
    std::vector<InQueue*> in_queues;
    std::vector<OutQueue*> out_queues;

  public:

    Mux();

    /** Waits and flushes queues until an input queue has messages or
     * eof pending.
     */
    void wait_and_flush();

  private:
    void create_fd_sets();

    int max_fd;
    fd_set read_set;
    fd_set write_set;
    std::vector<InQueue*> select_in_queues;
    std::vector<OutQueue*> select_out_queues;
  };

};

#endif /* MESSENGER_HH */
