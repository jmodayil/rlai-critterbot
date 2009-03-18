#ifndef DC1394Cam_H
#define DC1394Cam_H

#include "Component.h"
#include "USeconds.h"
//#include "ImageDrop.h"
#include "ColorImageDrop.h"
#include <dc1394/dc1394.h>
#include <SDL/SDL.h>

#define CAMERA_ID       0
#define CAM_SPEED       DC1394_ISO_SPEED_400
#define CAM_MODE        DC1394_VIDEO_MODE_640x480_RGB8 
//DC1394_VIDEO_MODE_FORMAT7_0
#define CAM_FRAMERATE   DC1394_FRAMERATE_15
#define CAM_BUF_SIZE    10
#define CAM_CAP_FLAGS   DC1394_CAPTURE_FLAGS_BANDWIDTH_ALLOC
// Format 7 features
#define CAM_WIDTH       640
#define CAM_HEIGHT      480
#define CAM_LEFT        0
#define CAM_TOP         0
#define CAM_PKT_SIZE    DC1394_USE_RECOMMENDED

#define getConfig( name , target ) \
  if( !tag->getAttrValue( name, target, err ) ) \
    return error(err.msg)

/**
 * A Component to get images from a firewire camera using libdc1394
 */
class DC1394Cam : public Component {
  public:
    DC1394Cam(DataLake *lake, ComponentConfig &config, string &name);
    virtual ~DC1394Cam();

    //for reloading config when it changes
    virtual int reloadConfig(ComponentConfig *config) ;

    //initial the device. does not block, will be called multiple times
    //if nessecary. return 0 if not done, negative on error, and positive when done
    virtual int init(USeconds &wokeAt);

    //read data, transform data, write new data
    virtual int think(USeconds &wokeAt);

    //returns the fid of a real input or real output device.
    //if the fid is negative, the device will be closed.
    virtual int getFID();

    //reads from the device to DataLake. receive() will only be called when data is present for RealInput
    //if the device wishes to be closed, return a negative number
    virtual int sense(USeconds &wokeAt);

    int thinks;

  private:

    int readConfig( ComponentConfig *config );
    RiverWrite imageDropID;

    ColorImageDrop *imageDrop;

    dc1394camera_t *camera;
    dc1394featureset_t features;
    dc1394framerate_t framerate;
    dc1394speed_t speed;
    dc1394video_mode_t mode;
    unsigned int width, height;
    unsigned int left, top;
    dc1394format7mode_t fmt7mode;
    dc1394video_frame_t *frame;
    dc1394_t *d;
    dc1394camera_list_t *list;
    dc1394error_t err;
    int camID;


    SDL_Surface *screen;

    int init_window( void );
    int display_image( void );
    void close_window( void );

    int init_camera( void );
    int capture_image ( void ); 
    void teardown_camera(dc1394camera_t *camera); 

    int fps;
    USeconds lastTime;
    bool display;
};

#endif
