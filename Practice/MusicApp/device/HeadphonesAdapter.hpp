#pragma once
#include "../models/Song.hpp"
#include "../external/HeadphonesAPI.hpp"
#include "IAudioOutputDevice.hpp"

class HeadphonesAdapter :public IAudioOutputDevice {
    private:
        HeadphonesAPI* headphonesApi;
    public:
        HeadphonesAdapter(HeadphonesAPI* api){
            headphonesApi = api;
        }
        void playAudio(Song* song){
            string payload = song->getTitle() + " by " + song->getArtist();
            headphonesApi->playSoundViaJack(payload);
        }
};