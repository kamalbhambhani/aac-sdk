/*
 * Copyright 2017-2018 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *     http://aws.amazon.com/apache2.0/
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

#include "AACE/Alexa/PlaybackController.h"

namespace aace {
namespace alexa {

PlaybackController::~PlaybackController() = default; // key function

void PlaybackController::playButtonPressed() {
    if( m_playbackControllerEngineInterface != nullptr ) {
        m_playbackControllerEngineInterface->onPlayButtonPressed();
    }
}

void PlaybackController::pauseButtonPressed() {
    if( m_playbackControllerEngineInterface != nullptr ) {
        m_playbackControllerEngineInterface->onPauseButtonPressed();
    }
}

void PlaybackController::nextButtonPressed() {
    if( m_playbackControllerEngineInterface != nullptr ) {
        m_playbackControllerEngineInterface->onNextButtonPressed();
    }
}

void PlaybackController::previousButtonPressed() {
    if( m_playbackControllerEngineInterface != nullptr ) {
        m_playbackControllerEngineInterface->onPreviousButtonPressed();
    }
}

void PlaybackController::setEngineInterface( std::shared_ptr<aace::alexa::PlaybackControllerEngineInterface> playbackControllerEngineInterface ) {
    m_playbackControllerEngineInterface = playbackControllerEngineInterface;
}

} // aace::alexa
} // aac
