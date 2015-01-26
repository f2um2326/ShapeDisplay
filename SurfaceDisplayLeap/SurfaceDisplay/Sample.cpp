/******************************************************************************\
* Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.               *
* Leap Motion proprietary and confidential. Not for distribution.              *
* Use subject to the terms of the Leap Motion SDK Agreement available at       *
* https://developer.leapmotion.com/sdk_agreement, or another agreement         *
* between Leap Motion and you, your company or other organization.             *
\******************************************************************************/


#include "Sample.h"

int gesture_flag = 0;
int bar_num = 0;
int current_height = 0;
int bar_heights[4][4] = {0};
int bar_start = 0;
int bar_num_flag[4][4] = {0};

const std::string fingerNames[] = {"Thumb", "Index", "Middle", "Ring", "Pinky"};
const std::string boneNames[] = {"Metacarpal", "Proximal", "Middle", "Distal"};
const std::string stateNames[] = {"STATE_INVALID", "STATE_START", "STATE_UPDATE", "STATE_END"};

void SampleListener::onInit(const Controller& controller) {
  std::cout << "Initialized" << std::endl;
}

void SampleListener::onConnect(const Controller& controller) {
  std::cout << "Connected" << std::endl;
  controller.enableGesture(Gesture::TYPE_CIRCLE);
  controller.enableGesture(Gesture::TYPE_KEY_TAP);
  controller.enableGesture(Gesture::TYPE_SCREEN_TAP);
  controller.enableGesture(Gesture::TYPE_SWIPE);
}

void SampleListener::onDisconnect(const Controller& controller) {
  // Note: not dispatched when running in a debugger.
  std::cout << "Disconnected" << std::endl;
}

void SampleListener::onExit(const Controller& controller) {
  std::cout << "Exited" << std::endl;
}

void SampleListener::onFrame(const Controller& controller) {
  // Get the most recent frame and report some basic information
  const Frame frame = controller.frame();
	/*
  std::cout << "Frame id: " << frame.id()
            << ", timestamp: " << frame.timestamp()
            << ", hands: " << frame.hands().count()
            << ", fingers: " << frame.fingers().count()
            << ", tools: " << frame.tools().count()
            << ", gestures: " << frame.gestures().count() << std::endl;
			*/
  HandList hands = frame.hands();
  for (HandList::const_iterator hl = hands.begin(); hl != hands.end(); ++hl) {
    // Get the first hand
    const Hand hand = *hl;
    //std::string handType = hand.isLeft() ? "Left hand" : "Right hand";

	// Grab Strength!!!
	float strength = hand.grabStrength();
	static float old_strength = 0;



	

	Leap::Vector position = hand.palmPosition();
	current_height = position.y;
	//std::cout << strength << ", " << position << std::endl;

	if(old_strength != 1 && strength == 1) { // Žè‚ð‚É‚¬‚Á‚½uŠÔ‚Ì‚P‰ñ
		// save current height as star position
		bar_start = current_height;
	} else if(old_strength == 1 && strength != 1) { // Žè‚ðŠJ‚¢‚½uŠÔ‚Ì‚P‰ñ
		// ƒtƒ‰ƒO‚ð—§‚Ä‚ÄCˆÚ“®•s‰Â”\‚É‚·‚éD

		if(bar_num == 30) {
			bar_num_flag[3][0] = 1;
		} else if(bar_num == 31) {
			bar_num_flag[3][1] = 1;
		} else if(bar_num == 32) {
			bar_num_flag[3][2] = 1;
		} else if(bar_num == 33) {
			bar_num_flag[3][3] = 1;
		} else if(bar_num == 20) {
			bar_num_flag[2][0] = 1;
		} else if(bar_num == 21) {
			bar_num_flag[2][1] = 1;
		} else if(bar_num == 22) {
			bar_num_flag[2][2] = 1;
		} else if(bar_num == 23) {
			bar_num_flag[2][3] = 1;
		} else if(bar_num == 10) {
			bar_num_flag[1][0] = 1;
		} else if(bar_num == 11) {
			bar_num_flag[1][1] = 1;
		} else if(bar_num == 12) {
			bar_num_flag[1][2] = 1;
		} else if(bar_num == 13) {
			bar_num_flag[1][3] = 1;
		} else if(bar_num == 0) {
			bar_num_flag[0][0] = 1;
		} else if(bar_num == 1) {
			bar_num_flag[0][1] = 1;
		} else if(bar_num == 2) {
			bar_num_flag[0][2] = 1;
		} else if(bar_num == 3) {
			bar_num_flag[0][3] = 1;
		}

		

	} else if(strength != 1) { // Žè‚ðŠJ‚¢‚Ä‚¢‚é‚Æ‚«‚¸‚Á‚Æ
		std::cout << bar_heights[3][0] << std::endl;

		// bar_num_flag[][] == 0‚È‚ç‚Îbar_heights‚ð‰Šú‰»
		for(int i = 0; i < 4; i++) {
			for(int j = 0; j < 4; j++) {
				if(bar_num_flag[i][j] == 0) bar_heights[i][j] = 0;
			}

		}


		if(position.z >= 50) { // 1st column from front
			if(position.x < -125) { // left side
				bar_num = 30;
				if(bar_num_flag[3][0] == 0) bar_heights[3][0] = 10;
			} else if(position.x >= -125 && position.x < -25) { // 2nd from left
				bar_num = 31;
				if(bar_num_flag[3][1] == 0) bar_heights[3][1] = 10;
			} else if(position.x >= -25 && position.x < 75) { // 3rd
				bar_num = 32;
				if(bar_num_flag[3][2] == 0) bar_heights[3][2] = 10;
			} else { //4 th
				bar_num = 33;
				if(bar_num_flag[3][3] == 0) bar_heights[3][3] = 10;
			}
		} else if(position.z < 50 && position.z >= -50) { // 2nd column
			if(position.x < -75) { // left side
				bar_num = 20;
				if(bar_num_flag[2][0] == 0) bar_heights[2][0] = 10;
			} else if(position.x >= -75 && position.x < 25) { // 2nd from left
				bar_num = 21;
				if(bar_num_flag[2][1] == 0) bar_heights[2][1] = 10;
			} else if(position.x >= 25 && position.x < 125) { // 3rd
				bar_num = 22;
				if(bar_num_flag[2][2] == 0) bar_heights[2][2] = 10;
			} else { //4 th
				bar_num = 23;
				if(bar_num_flag[2][3] == 0) bar_heights[2][3] = 10;
			}
		} else if(position.z < -50 && position.z >= -150) { // 3rd column
			if(position.x < -125) { // left side
				bar_num = 10;
				if(bar_num_flag[1][0] == 0) bar_heights[1][0] = 10;
			} else if(position.x >= -125 && position.x < -25) { // 2nd from left
				bar_num = 11;
				if(bar_num_flag[1][1] == 0) bar_heights[1][1] = 10;
			} else if(position.x >= -25 && position.x < 75) { // 3rd
				bar_num = 12;
				if(bar_num_flag[1][2] == 0) bar_heights[1][2] = 10;
			} else { //4 th
				bar_num = 13;
				if(bar_num_flag[1][3] == 0) bar_heights[1][3] = 10;
			}
		} else { //4th column
			if(position.x < -75) { // left side
				bar_num = 0;
				if(bar_num_flag[0][0] == 0) bar_heights[0][0] = 10;
			} else if(position.x >= -75 && position.x < 25) { // 2nd from left
				bar_num = 1;
				if(bar_num_flag[0][1] == 0) bar_heights[0][1] = 10;
			} else if(position.x >= 25 && position.x < 125) { // 3rd
				bar_num = 2;
				if(bar_num_flag[0][2] == 0) bar_heights[0][2] = 10;
			} else { //4 th
				bar_num = 3;
				if(bar_num_flag[0][3] == 0) bar_heights[0][3] = 10;
			}
		}
	} else if(strength == 1) { // Žè‚ðˆ¬‚Á‚Ä‚¢‚é‚Æ‚«‚¸‚Á‚Æ
		if(bar_num == 30 && bar_num_flag[3][0] == 0) bar_heights[3][0] = current_height - bar_start;
		else if(bar_num == 31 && bar_num_flag[3][1] == 0) bar_heights[3][1] = current_height - bar_start;
		else if(bar_num == 32 && bar_num_flag[3][2] == 0) bar_heights[3][2] = current_height - bar_start;
		else if(bar_num == 33 && bar_num_flag[3][3] == 0) bar_heights[3][3] = current_height - bar_start;

		else if(bar_num == 20 && bar_num_flag[2][0] == 0) bar_heights[2][0] = current_height - bar_start;
		else if(bar_num == 21 && bar_num_flag[2][1] == 0) bar_heights[2][1] = current_height - bar_start;
		else if(bar_num == 22 && bar_num_flag[2][2] == 0) bar_heights[2][2] = current_height - bar_start;
		else if(bar_num == 23 && bar_num_flag[2][3] == 0) bar_heights[2][3] = current_height - bar_start;

		else if(bar_num == 10 && bar_num_flag[1][0] == 0) bar_heights[1][0] = current_height - bar_start;
		else if(bar_num == 11 && bar_num_flag[1][1] == 0) bar_heights[1][1] = current_height - bar_start;
		else if(bar_num == 12 && bar_num_flag[1][2] == 0) bar_heights[1][2] = current_height - bar_start;
		else if(bar_num == 13 && bar_num_flag[1][3] == 0) bar_heights[1][3] = current_height - bar_start;

		else if(bar_num == 0 && bar_num_flag[0][0] == 0) bar_heights[0][0] = current_height - bar_start;
		else if(bar_num == 1 && bar_num_flag[0][1] == 0) bar_heights[0][1] = current_height - bar_start;
		else if(bar_num == 2 && bar_num_flag[0][2] == 0) bar_heights[0][2] = current_height - bar_start;
		else if(bar_num == 3 && bar_num_flag[0][3] == 0) bar_heights[0][3] = current_height - bar_start;
	}
	old_strength = strength;



	/*
    std::cout << std::string(2, ' ') << handType << ", id: " << hand.id()
              << ", palm position: " << hand.palmPosition() << std::endl;
			  */
    // Get the hand's normal vector and direction
    const Vector normal = hand.palmNormal();
    const Vector direction = hand.direction();

    // Calculate the hand's pitch, roll, and yaw angles
	/*
    std::cout << std::string(2, ' ') <<  "pitch: " << direction.pitch() * RAD_TO_DEG << " degrees, "
              << "roll: " << normal.roll() * RAD_TO_DEG << " degrees, "
              << "yaw: " << direction.yaw() * RAD_TO_DEG << " degrees" << std::endl;
			  */

    // Get fingers
    const FingerList fingers = hand.fingers();
    for (FingerList::const_iterator fl = fingers.begin(); fl != fingers.end(); ++fl) {
      const Finger finger = *fl;
	  
	  /*
      std::cout << std::string(4, ' ') <<  fingerNames[finger.type()]
                << " finger, id: " << finger.id()
                << ", length: " << finger.length()
                << "mm, width: " << finger.width() << std::endl;
		*/
				
      // Get finger bones
      for (int b = 0; b < 4; ++b) {
        Bone::Type boneType = static_cast<Bone::Type>(b);
        Bone bone = finger.bone(boneType);
		/*
        std::cout << std::string(6, ' ') <<  boneNames[boneType]
                  << " bone, start: " << bone.prevJoint()
                  << ", end: " << bone.nextJoint()
                  << ", direction: " << bone.direction() << std::endl;
				  */
      }
    }
  }

  // Get tools
  /*
  const ToolList tools = frame.tools();
  for (ToolList::const_iterator tl = tools.begin(); tl != tools.end(); ++tl) {
    const Tool tool = *tl;
	
    std::cout << std::string(2, ' ') <<  "Tool, id: " << tool.id()
              << ", position: " << tool.tipPosition()
              << ", direction: " << tool.direction() << std::endl;
			  
  }
  */

  // Get gestures
  
  const GestureList gestures = frame.gestures();
  for (int g = 0; g < gestures.count(); ++g) {
    Gesture gesture = gestures[g];

    switch (gesture.type()) {
		/*
      case Gesture::TYPE_CIRCLE:
      {
        CircleGesture circle = gesture;
        std::string clockwiseness;

        if (circle.pointable().direction().angleTo(circle.normal()) <= PI/4) {
          clockwiseness = "clockwise";
        } else {
          clockwiseness = "counterclockwise";
        }

        // Calculate angle swept since last frame
        float sweptAngle = 0;
        if (circle.state() != Gesture::STATE_START) {
          CircleGesture previousUpdate = CircleGesture(controller.frame(1).gesture(circle.id()));
          sweptAngle = (circle.progress() - previousUpdate.progress()) * 2 * PI;
        }
        std::cout << std::string(2, ' ')
                  << "Circle id: " << gesture.id()
                  << ", state: " << stateNames[gesture.state()]
                  << ", progress: " << circle.progress()
                  << ", radius: " << circle.radius()
                  << ", angle " << sweptAngle * RAD_TO_DEG
                  <<  ", " << clockwiseness << std::endl;
        break;
      }
	  */
      case Gesture::TYPE_SWIPE:
      {
        SwipeGesture swipe = gesture;

		if(swipe.direction().x <= 0 && gesture_flag != LEFT_SWIPE) {
			std::cout << "Left" << std::endl;
			gesture_flag = LEFT_SWIPE;

			for(int i = 0; i < 4; i++) {
				for(int j = 0; j < 4; j++) {
					bar_num_flag[i][j] = 0;
					bar_heights[i][j] = 0;
				}
			}

		} else if (swipe.direction().x > 0 && gesture_flag != RIGHT_SWIPE) {
			std::cout << "Right" << std::endl;
			gesture_flag = RIGHT_SWIPE;

			for(int i = 0; i < 4; i++) {
				for(int j = 0; j < 4; j++) {
					bar_num_flag[i][j] = 0;
					bar_heights[i][j] = 0;
				}
			}

		}

		/*
        std::cout << std::string(2, ' ')
          << "Swipe id: " << gesture.id()
          //<< ", state: " << stateNames[gesture.state()]
          << ", direction: " << swipe.direction()
          //<< ", speed: " << swipe.speed() 
		  << std::endl;
		  */
        break;
      }
	  /*
      case Gesture::TYPE_KEY_TAP:
      {
        KeyTapGesture tap = gesture;
        std::cout << std::string(2, ' ')
          << "Key Tap id: " << gesture.id()
          << ", state: " << stateNames[gesture.state()]
          << ", position: " << tap.position()
          << ", direction: " << tap.direction()<< std::endl;
        break;
      }

      case Gesture::TYPE_SCREEN_TAP:
      {
        ScreenTapGesture screentap = gesture;
        std::cout << std::string(2, ' ')
          << "Screen Tap id: " << gesture.id()
          << ", state: " << stateNames[gesture.state()]
          << ", position: " << screentap.position()
          << ", direction: " << screentap.direction()<< std::endl;
        break;
      }
	  */
      default:
        //std::cout << std::string(2, ' ')  << "Unknown gesture type." << std::endl;
        break;
    }
  }
  

  if (!frame.hands().isEmpty() || !gestures.isEmpty()) {
    
	//std::cout << std::endl;
	
  }
  
}

void SampleListener::onFocusGained(const Controller& controller) {
  std::cout << "Focus Gained" << std::endl;
}

void SampleListener::onFocusLost(const Controller& controller) {
  std::cout << "Focus Lost" << std::endl;
}

void SampleListener::onDeviceChange(const Controller& controller) {
  std::cout << "Device Changed" << std::endl;
  const DeviceList devices = controller.devices();

  for (int i = 0; i < devices.count(); ++i) {
    //std::cout << "id: " << devices[i].toString() << std::endl;
    //std::cout << "  isStreaming: " << (devices[i].isStreaming() ? "true" : "false") << std::endl;
  }
}

void SampleListener::onServiceConnect(const Controller& controller) {
  std::cout << "Service Connected" << std::endl;
}

void SampleListener::onServiceDisconnect(const Controller& controller) {
  std::cout << "Service Disconnected" << std::endl;
}


