#pragma once

// #include <string>
// #include <vector>
// #include <stdlib.h>
// #include <inttypes.h>
// #include <opencv2/core.hpp>

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>

// #include "cuda/onoff.h"
// #include "cctag/params.hpp"
// #include "cctag/types.hpp"
// #include "cctag/ImageCut.hpp"
// #include "cctag/geometry/Ellipse.hpp"
// #include "cctag/geometry/point.hpp"
// #include "cctag/algebra/matrix/Matrix.hpp"
// #include "cctag/logtime.hpp"

namespace popart
{

class TagPipe;
class TagThreads;

class TagSemaphore
{
    int              _sema_val;
    boost::mutex     _sema_lock;
    boost::condition _sema_cond;
public:
    TagSemaphore( int init )
        : _sema_val( init )
    { }

    inline void wait( int n = 1 ) {
        _sema_lock.lock();
        while( _sema_val - n < 0 ) _sema_cond.wait( _sema_lock );
        _sema_val -= n;
        _sema_lock.unlock();
    }
    inline void post( int n = 1 ) {
        _sema_lock.lock();
        _sema_val += n;
        _sema_cond.notify_all();
        _sema_lock.unlock();
    }
};

class TagThread : public boost::thread
{
    TagThreads* _creator;
    TagPipe*    _pipe;
    int         _my_layer;

public:
    TagThread( TagThreads* creator, TagPipe* pipe, int layer );

    void call( void );
};

class TagThreads
{
    TagPipe*         _pipe;
    int              _layers;
    TagSemaphore     _start;
    TagSemaphore     _frameReady;
    TagSemaphore     _frameDone;
public:
    TagThreads( );

    void init( TagPipe* pipe, int layers );

    void oneRound( );

    void startWait( );
    void startPost( );
    void frameReadyWait( );
    void frameReadyPost( );
    void frameDoneWait( );
    void frameDonePost( );
};

}; // namespace popart
