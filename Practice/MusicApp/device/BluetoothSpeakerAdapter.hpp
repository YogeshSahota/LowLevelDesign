#pragma once
#include "../models/Song.hpp"
#include "../external/BluetoothSpeakerAPI.hpp"
#include "IAudioOutputDevice.hpp"

class BluetoothSpeakerAdapter :public IAudioOutputDevice {
    private:
        BluetoothSpeakerAPI* bluetoothApi;
    public:
        BluetoothSpeakerAdapter(BluetoothSpeakerAPI* api){
            bluetoothApi = api;
        }
        void playAudio(Song* song){
            string payload = song->getTitle() + " by " + song->getArtist();
            bluetoothApi->playSoundViaBluetooth(payload);
        }
};