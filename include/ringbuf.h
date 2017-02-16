 #ifndef __RINGBUFS_H
    #define __RINGBUFS_H
    #define RBUF_SIZE    5

    typedef struct  ringBufS
    {
      unsigned char buf[RBUF_SIZE];
      int head;
      int tail;
      int count;
    } ringBufS;

    #ifdef  __cplusplus
      extern  "C" {
    #endif
      void  ringBufS_init  (ringBufS *_this);
      int   ringBufS_empty (ringBufS *_this);
      int   ringBufS_full  (ringBufS *_this);
      int   ringBufS_get(ringBufS *_this);
      int 	ringBufS_available(ringBufS *_this);
      void  ringBufS_put   (ringBufS *_this, const unsigned char c);
      void  ringBufS_flush (ringBufS *_this, const int clearBuffer);
    #ifdef  __cplusplus
      }
    #endif
#endif
