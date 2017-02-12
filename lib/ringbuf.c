 
#include  <string.h>
#include  "ringbuf.h"

void ringBufS_init (ringBufS *_this)
{
    /*****
      The following clears:
        -> buf
        -> head
        -> tail
        -> count
      and sets head = tail
    ***/
    memset (_this, 0, sizeof (*_this));
}


int ringBufS_empty (ringBufS *_this)
{
    return (0==_this->count);
}

int ringBufS_available (ringBufS *_this)
{
	return (_this->count);
}

int ringBufS_full (ringBufS *_this)
{
    return (_this->count>=RBUF_SIZE);
}



int ringBufS_get(ringBufS *_this)
{
    int c;
    if (_this->count>0)
    {
      c           = _this->buf[_this->tail];
      _this->tail = (_this->tail%RBUF_SIZE)+1;
      --_this->count;
    }
    else
    {
      c = -1;
    }
    return (c);
}


void ringBufS_put (ringBufS *_this, const unsigned char c)
{
    if (_this->count < RBUF_SIZE)
    {
      _this->buf[_this->head] = c;
      _this->head = (_this->head%RBUF_SIZE)+1;
      ++_this->count;
    }
}


void ringBufS_flush (ringBufS *_this, const int clearBuffer)
{
  _this->count  = 0;
  _this->head   = 0;
  _this->tail   = 0;
  if (clearBuffer)
  {
    memset (_this->buf, 0, sizeof (_this->buf));
  }
}
