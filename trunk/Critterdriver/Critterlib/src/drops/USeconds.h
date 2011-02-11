#ifndef MICRO_SECONDS_H
#define MICRO_SECONDS_H

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#define ONE_MILLION 1000000

//! DataDrop representing micro seconds
/*!
Used for timing
*/
class USeconds {
  public:
    //! time.tv_usec is micro seconds and time.tv_sec is seconds
    struct timeval time;

    //! Constructor
    USeconds();
    //! Copy Contructor
    USeconds(const USeconds &copy);
    //! Constructor with predefined milliseconds
    USeconds(unsigned int milliseconds);

    //! DataDrop::getSize()
    virtual int getSize() const;
    //! DataDrop::writeArray()
    virtual void writeArray(void *data) const;
    //! DataDrop::readArray()
    virtual void readArray(void *data);
    //! Do a system call and set the time to now
    void setAsNow();
};
#endif
