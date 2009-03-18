#include "DC1394Cam.h"
#include "ErrorMessage.h"
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

DC1394Cam::DC1394Cam(DataLake *lake, 
                                 ComponentConfig &config, 
                                 string &name) : 
                                 Component(lake, config, name) {
  
  imageDrop = new ColorImageDrop();
  thinks = 0;
  imageDropID = lake->readyWriting("ColorImageDrop");
}

DC1394Cam::~DC1394Cam() {
  if( imageDrop )
    free(imageDrop);
  close_window();
  teardown_camera( camera );
  lake->doneWriteHead(imageDropID);
}

int DC1394Cam::readConfig(ComponentConfig *config) {

  XMLNode *tag;
  XMLError err;

  tag = config->getXMLNode("config", err);
  if( tag == NULL ) 
    return error(err.msg);

  getConfig("displayVideo", display);

  return 1;
}

int DC1394Cam::reloadConfig(ComponentConfig *config) {

  this->config = *config;
  readConfig( config );
  
}

int DC1394Cam::init_camera( void ) {

  d = dc1394_new();
  
  if( !d ) {
    error("Failed to open bus - Dying");
    return -1;
  }
  if( dc1394_camera_enumerate( d, &list ) != DC1394_SUCCESS ) {
    error("Failed to enumerate cameras - Dying");
    return -1;
  }
  if( list->num < CAMERA_ID ) {
    error("No camera with ID %d on bus. - Dying", CAMERA_ID);
    return -1;
  }

  camera = dc1394_camera_new( d, list->ids[CAMERA_ID].guid );
  if( !camera ) {
    error("Failed to initialize camera ID %d. -Dying", CAMERA_ID);
    return -1;
  }
  dc1394_camera_free_list( list );

  debug( "Opened camera with ID %d, GUID %x", CAMERA_ID, camera->guid ); 

  if( dc1394_video_set_iso_speed( camera, CAM_SPEED ) != DC1394_SUCCESS ) {
    error("Failed to set iso speed. - Dying");
    teardown_camera( camera );
    return -1;
  }   
  if( dc1394_video_set_mode( camera, CAM_MODE ) != DC1394_SUCCESS ) {
    error("Failed to set camera mode. - Dying");
    teardown_camera( camera );
    return -1;
  }
  /*
  if( dc1394_format7_set_image_size( camera, CAM_MODE, CAM_WIDTH, CAM_HEIGHT ) 
      != DC1394_SUCCESS ) {
    error("Failed to set image size. - Dying");
    teardown_camera( camera );
    return -1;
  }   
  if( dc1394_format7_set_image_position( camera, CAM_MODE, CAM_LEFT, CAM_TOP )
      != DC1394_SUCCESS ) {
    error("Failed to set image position. - Dying");
    teardown_camera( camera );
    return -1;
  }   
  if( dc1394_format7_set_packet_size( camera, CAM_MODE, CAM_PKT_SIZE ) 
      != DC1394_SUCCESS ) {
    error("Failed to set packet size. - Dying");
    teardown_camera( camera );
    return -1;
  } */  

  // Not needed for FORMAT7 ?
  if( dc1394_video_set_framerate( camera, CAM_FRAMERATE ) != DC1394_SUCCESS ) {
    error("Failed to set framerate. - Dying");
    teardown_camera( camera );
    return -1;
  }   
  dc1394_feature_get_all( camera, &features );
  dc1394_feature_print_all( &features, stdout );

  dc1394_feature_set_value( camera, DC1394_FEATURE_SHUTTER, 200 );
  if( dc1394_capture_setup( camera, CAM_BUF_SIZE, CAM_CAP_FLAGS ) != 
      DC1394_SUCCESS ) {
    error("Failed to setup capture. - Dying");
    teardown_camera( camera );
    return -1;
  }   
  
  if( dc1394_video_set_transmission( camera, DC1394_ON ) != DC1394_SUCCESS ) {
    error("Failed to start iso transmission. - Dying");
    teardown_camera( camera );
    return -1;
  }   

  debug("Started camera successfully.");
  return 1;
}

int DC1394Cam::init_window( void ) {
  
  if( SDL_Init(SDL_INIT_VIDEO) != 0 ) {
    error("Unable to initialize SDL: %s", SDL_GetError());
    return -1;
  }
  atexit(SDL_Quit);

  if( screen ) 
    return -1;
  screen = SDL_SetVideoMode( 640, 480, 24, SDL_DOUBLEBUF );
  if( screen == NULL ) {
    error("Unable to set video mode: %s", SDL_GetError());
    return -1;
  }
  
  debug("Initialized display context.");
  return 1;
}

int DC1394Cam::init(USeconds &wokeAt) {
  if( readConfig( &config ) != 1 )
    return -1; 
  if(display)
    init_window();
  return init_camera(); 
}

int DC1394Cam::capture_image( void ) {
  
  int result = dc1394_capture_dequeue( camera, DC1394_CAPTURE_POLICY_POLL, &frame );
  if( result != DC1394_SUCCESS) {
    error("Unable to capture frame from camera!");
    return -1;
  }
  if( frame == NULL )
    return 0;
  //debug("Got 1 frame from camera."); 
  dc1394_capture_enqueue( camera, frame );
  return 1;
}

void DC1394Cam::close_window( void ) {
  if( screen )
    SDL_FreeSurface( screen );
}

void DC1394Cam::teardown_camera(dc1394camera_t *camera) {

  dc1394_video_set_transmission( camera, DC1394_OFF );
  dc1394_capture_stop( camera );
  if( camera )
    dc1394_camera_free( camera );
  if( d )
    dc1394_free( d );
}

int DC1394Cam::display_image( void ) {

  unsigned char bufi[3];

  SDL_LockSurface( screen ); 

  int size = frame->size[0] * frame->size[1];

  for( int i = 0; i < size * 3; i += 3 ) {
    bufi[0] = frame->image[i+2];
    bufi[1] = frame->image[i+1];
    bufi[2] = frame->image[i];
    memcpy( (unsigned char*)screen->pixels + i, bufi, 3);
    //memset( (unsigned char*)screen->pixels + i * 3, frame->image[i], 3 );
  }
  //memcpy( screen->pixels, frame->image, size * 3 );
  SDL_UnlockSurface( screen );

  SDL_Flip( screen );
  return 0;
}

int DC1394Cam::think(USeconds &wokeAt) {
  if( capture_image() == 1) {
    fps++;
    imageDrop->width = frame->size[0];
    imageDrop->height = frame->size[1];
    imageDrop->seqNum++;
    imageDrop->setImage(frame->image);
    (*(ColorImageDrop*)lake->startWriteHead(imageDropID)) = *imageDrop;  
    lake->doneWriteHead(imageDropID);
    if(display)
      display_image(); 
  }
  if(lastTime.time.tv_sec != wokeAt.time.tv_sec) {
    debug("FPS: %d", fps);
    fps = 0;
  }

  lastTime = wokeAt;
  thinks++;
  return 0;
}

int DC1394Cam::getFID(){
  return -1;
}

int DC1394Cam::sense(USeconds &wokeAt){
  senses++;
  return -1;
}
