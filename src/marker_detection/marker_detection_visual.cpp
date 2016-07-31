/*
 * Copyright (c) 2012, Willow Garage, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Willow Garage, Inc. nor the names of its
 *       contributors may be used to endorse or promote products derived from
 *       this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <OGRE/OgreVector3.h>
#include <OGRE/OgreSceneNode.h>
#include <OGRE/OgreSceneManager.h>

#include "marker_detection/marker_detection_visual.h"
#include "rviz/ogre_helpers/axes.h"

namespace marker_rviz_plugin {

MarkerDetectionVisual::MarkerDetectionVisual ( Ogre::SceneManager* scene_manager, Ogre::SceneNode* parent_node ) {
    scene_manager_ = scene_manager;

    // Ogre::SceneNode s form a tree, with each node storing the
    // transform (position and orientation) of itself relative to its
    // parent.  Ogre does the math of combining those transforms when it
    // is time to render.
    //
    // Here we create a node to store the pose of the MarkerDetection's header frame
    // relative to the RViz fixed frame.
    frame_node_ = parent_node->createChildSceneNode();

    // initialize global variables
    color_ = Ogre::ColourValue ( 255, 0, 0 );
    shape_type_ = MarkerShape::Cube;
    scale_ = 0.0;
}

MarkerDetectionVisual::~MarkerDetectionVisual() {
    // Destroy the frame node since we don't need it anymore.
    scene_manager_->destroySceneNode ( frame_node_ );
}

void MarkerDetectionVisual::setMessage ( const marker_msgs::MarkerDetection::ConstPtr& msg ) {
    markers_.resize ( msg->markers.size() );
    for ( size_t i = 0; i < markers_.size(); i++ ) {
        double p_x = msg->markers[i].pose.position.x;
        double p_y = msg->markers[i].pose.position.y;
        double p_z = msg->markers[i].pose.position.z;
        double o_x = msg->markers[i].pose.orientation.x;
        double o_y = msg->markers[i].pose.orientation.y;
        double o_z = msg->markers[i].pose.orientation.z;
        double o_w = msg->markers[i].pose.orientation.w;

        if(shape_type_ == MarkerShape::Marker){
            rviz::Axes *a = new rviz::Axes( scene_manager_, frame_node_ );
            markers_[i].reset( a->getXShape() );
        }else{
            markers_[i].reset ( new rviz::Shape ( rviz::Shape::Cube, scene_manager_, frame_node_ ) );
        }
        markers_[i]->setColor ( color_ );
        markers_[i]->setPosition ( Ogre::Vector3 ( p_x, p_y, p_z ) );
        markers_[i]->setOrientation ( Ogre::Quaternion ( o_w, o_x, o_y, o_z ) );
        markers_[i]->setScale ( Ogre::Vector3 ( scale_, scale_, scale_ ) );
    }
}

// Position is passed through to the SceneNode.
void MarkerDetectionVisual::setFramePosition ( const Ogre::Vector3& position ) {
    frame_node_->setPosition ( position );
}

// Orientation is passed through to the SceneNode.
void MarkerDetectionVisual::setFrameOrientation ( const Ogre::Quaternion& orientation ) {
    frame_node_->setOrientation ( orientation );
}

// Color is passed through to the Shape object.
void MarkerDetectionVisual::setColor ( Ogre::ColourValue color ) {
    for ( size_t i = 0; i < markers_.size(); i++ ) {
        markers_[i]->setColor ( color );
    }
    color_ = color;
}

// Shape type is passed through to the Shape object.
void MarkerDetectionVisual::setShape ( MarkerShape::Type shape_type ) {
    for ( size_t i = 0; i < markers_.size(); i++ ) {
        Ogre::Vector3 position = markers_[i]->getPosition();
        Ogre::Quaternion orientation = markers_[i]->getOrientation();

        if(shape_type == MarkerShape::Marker){
            rviz::Axes *a = new rviz::Axes( scene_manager_, frame_node_ );
            markers_[i].reset( a->getXShape() );
        }else{
            markers_[i].reset ( new rviz::Shape ( rviz::Shape::Cube, scene_manager_, frame_node_ ) );
        }
        markers_[i]->setColor ( color_ );
        markers_[i]->setPosition ( position );
        markers_[i]->setOrientation ( orientation );
        markers_[i]->setScale ( Ogre::Vector3 ( scale_, scale_, scale_ ) );
    }

    shape_type_ = shape_type;
}

// Scale is passed through to the Shape object.
void MarkerDetectionVisual::setScale ( float scale ) {
    for ( size_t i = 0; i < markers_.size(); i++ ) {
        markers_[i]->setScale ( Ogre::Vector3 ( scale_, scale_, scale_ ) );
    }
    scale_ = scale;
}

} // end namespace marker_rviz_plugin

