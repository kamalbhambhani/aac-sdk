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

#ifndef AACE_ENGINE_ALEXA_SPEECH_RECOGNIZER_ENGINE_IMPL_H
#define AACE_ENGINE_ALEXA_SPEECH_RECOGNIZER_ENGINE_IMPL_H

#include <memory>
#include <string>

#include <AIP/AudioInputProcessor.h>
#include <AIP/AudioProvider.h>
#include <AIP/Initiator.h>
#include <AVSCommon/AVS/DialogUXStateAggregator.h>
#include <AVSCommon/SDKInterfaces/AudioInputProcessorObserverInterface.h>
#include <AVSCommon/SDKInterfaces/DirectiveSequencerInterface.h>
#include <AVSCommon/SDKInterfaces/CapabilitiesDelegateInterface.h>
#include <AVSCommon/SDKInterfaces/ExceptionEncounteredSenderInterface.h>
#include <AVSCommon/SDKInterfaces/FocusManagerInterface.h>
#include <AVSCommon/SDKInterfaces/KeyWordObserverInterface.h>
#include <AVSCommon/SDKInterfaces/MessageSenderInterface.h>
#include <ContextManager/ContextManager.h>
#include <KWD/AbstractKeywordDetector.h>

#if defined AMAZONLITE_WAKEWORD_SUPPORT
#include <AmazonLite/PryonLiteKeywordDetector.h>
#endif

#include "AACE/Alexa/SpeechRecognizer.h"

namespace aace {
namespace engine {
namespace alexa {

class SpeechRecognizerEngineImpl :
    public aace::alexa::SpeechRecognizerEngineInterface,
    public alexaClientSDK::avsCommon::sdkInterfaces::AudioInputProcessorObserverInterface,
    public alexaClientSDK::avsCommon::sdkInterfaces::KeyWordObserverInterface,
    public alexaClientSDK::avsCommon::utils::RequiresShutdown,
    public std::enable_shared_from_this<SpeechRecognizerEngineImpl> {

private:
    SpeechRecognizerEngineImpl( std::shared_ptr<aace::alexa::SpeechRecognizer> speechRecognizerPlatformInterface, const alexaClientSDK::avsCommon::utils::AudioFormat& audioFormat );

    bool initialize(
        std::shared_ptr<alexaClientSDK::avsCommon::sdkInterfaces::DirectiveSequencerInterface> directiveSequencer,
        std::shared_ptr<alexaClientSDK::avsCommon::sdkInterfaces::MessageSenderInterface> messageSender,
        std::shared_ptr<alexaClientSDK::avsCommon::sdkInterfaces::ContextManagerInterface> contextManager,
        std::shared_ptr<alexaClientSDK::avsCommon::sdkInterfaces::FocusManagerInterface> focusManager,
        std::shared_ptr<alexaClientSDK::avsCommon::avs::DialogUXStateAggregator> dialogUXStateAggregator,
        std::shared_ptr<alexaClientSDK::avsCommon::sdkInterfaces::CapabilitiesDelegateInterface> capabilitiesDelegate,
        std::shared_ptr<alexaClientSDK::avsCommon::sdkInterfaces::ExceptionEncounteredSenderInterface> exceptionSender,
        std::shared_ptr<alexaClientSDK::avsCommon::sdkInterfaces::UserActivityNotifierInterface> userActivityNotifier );

public:
    static std::shared_ptr<SpeechRecognizerEngineImpl> create(
        std::shared_ptr<aace::alexa::SpeechRecognizer> speechRecognizerPlatformInterface,
        const alexaClientSDK::avsCommon::utils::AudioFormat& audioFormat,
        std::shared_ptr<alexaClientSDK::avsCommon::sdkInterfaces::DirectiveSequencerInterface> directiveSequencer,
        std::shared_ptr<alexaClientSDK::avsCommon::sdkInterfaces::MessageSenderInterface> messageSender,
        std::shared_ptr<alexaClientSDK::avsCommon::sdkInterfaces::ContextManagerInterface> contextManager,
        std::shared_ptr<alexaClientSDK::avsCommon::sdkInterfaces::FocusManagerInterface> focusManager,
        std::shared_ptr<alexaClientSDK::avsCommon::avs::DialogUXStateAggregator> dialogUXStateAggregator,
        std::shared_ptr<alexaClientSDK::avsCommon::sdkInterfaces::CapabilitiesDelegateInterface> capabilitiesDelegate,
        std::shared_ptr<alexaClientSDK::avsCommon::sdkInterfaces::ExceptionEncounteredSenderInterface> exceptionSender,
        std::shared_ptr<alexaClientSDK::avsCommon::sdkInterfaces::UserActivityNotifierInterface> userActivityNotifier );

    // SpeechRecognizerEngineInterface
    bool onHoldToTalk() override;
    bool onTapToTalk() override;
    bool onStopCapture() override;

    ssize_t write( const int16_t* data, const size_t size ) override;

    // keyword detection
    bool isWakewordEnabled();
    bool isWakewordSupported();

    bool enableWakewordDetection() override;
    bool disableWakewordDetection() override;

    // AudioInputProcessorObserverInterface
    void onStateChanged( alexaClientSDK::avsCommon::sdkInterfaces::AudioInputProcessorObserverInterface::State state ) override;

    // KeyWordObserverInterface
    void onKeyWordDetected(
        std::shared_ptr<alexaClientSDK::avsCommon::avs::AudioInputStream> stream,
        std::string keyword,
        alexaClientSDK::avsCommon::avs::AudioInputStream::Index beginIndex = UNSPECIFIED_INDEX,
        alexaClientSDK::avsCommon::avs::AudioInputStream::Index endIndex = UNSPECIFIED_INDEX,
        std::shared_ptr<const std::vector<char>> KWDMetadata = nullptr ) override;

protected:
    virtual void doShutdown() override;

private:
    bool startCapture( std::shared_ptr<alexaClientSDK::capabilityAgents::aip::AudioProvider> audioProvider,
                        alexaClientSDK::capabilityAgents::aip::Initiator initiator,
                        alexaClientSDK::avsCommon::avs::AudioInputStream::Index begin = alexaClientSDK::capabilityAgents::aip::AudioInputProcessor::INVALID_INDEX,
                        alexaClientSDK::avsCommon::avs::AudioInputStream::Index keywordEnd = alexaClientSDK::capabilityAgents::aip::AudioInputProcessor::INVALID_INDEX,
                        const std::string& keyword = "" );
    
    bool initializeAudioInputStream();

    void setExpectingAudioState( bool state );
    bool waitForExpectingAudioState( bool state, const std::chrono::seconds duration = std::chrono::seconds( 3 ) );

private:
    std::shared_ptr<aace::alexa::SpeechRecognizer> m_speechRecognizerPlatformInterface;
    std::shared_ptr<alexaClientSDK::capabilityAgents::aip::AudioInputProcessor> m_audioInputProcessor;
    
    alexaClientSDK::avsCommon::utils::AudioFormat m_audioFormat;
    std::shared_ptr<alexaClientSDK::avsCommon::avs::AudioInputStream> m_audioInputStream;
    std::unique_ptr<alexaClientSDK::avsCommon::avs::AudioInputStream::Writer> m_audioInputWriter;
    
    unsigned int m_wordSize;
    
    std::shared_ptr<alexaClientSDK::kwd::AbstractKeywordDetector> m_wakewordDetector;
    
    bool m_expectingAudio = false;
    bool m_wakewordEnabled = false;
    
    // the aip state
    AudioInputProcessorObserverInterface::State m_state;
    
    // mutex for blocking
    std::mutex m_expectingAudioMutex;
    std::condition_variable m_expectingAudioState_cv;

};

} // aace::engine::alexa
} // aace::engine
} // aace

#endif // AACE_ENGINE_ALEXA_SPEECH_RECOGNIZER_ENGINE_IMPL_H
