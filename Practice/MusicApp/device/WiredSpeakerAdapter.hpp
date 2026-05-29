#pragma once
#include "../models/Song.hpp"
#include "../external/WiredSpeakerAPI.hpp"
#include "IAudioOutputDevice.hpp"

class WiredSpeakerAdapter :public IAudioOutputDevice {
    private:
        WiredSpeakerAPI* wiredApi;
    public:
        WiredSpeakerAdapter(WiredSpeakerAPI* api){
            wiredApi = api;
        }
        void playAudio(Song* song){
            string payload = song->getTitle() + " by " + song->getArtist();
            wiredApi->playSoundViaCable(payload);
        }
};