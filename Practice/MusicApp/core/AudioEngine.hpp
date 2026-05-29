#pragma once

#include <string>
#include <iostream>
#include "../models/Song.hpp"
#include "../device/IAudioOutputDevice.hpp"

using namespace std;

class AudioEngine {
    private:
        Song*  currentSong;
        bool songIsPaused;
    
    public:
        AudioEngine(){
            currentSong = nullptr;
            songIsPaused = false;
        }

        string getCurrentSongTitle(){
            if(currentSong){
                return currentSong->getTitle();
            }
            return "";
        }

        bool isPaused(){
            return songIsPaused;
        }

        void play(IAudioOutputDevice* aod, Song* song){
            if(song == nullptr){
                throw runtime_error("Cannot play a null song.");
            }

            if(songIsPaused && currentSong == song){
                songIsPaused = false;
                cout << "Resuming song: " << song->getTitle() << "\n";
                aod->playAudio(song);
                return;
            }

            currentSong=song;
            songIsPaused=false;
            cout << "Playing song: " << song->getTitle() << "\n";
            aod->playAudio(song);
            return;
        }

        void pause(){
            if (currentSong == nullptr) {
                throw runtime_error("No song is currently playing to pause.");
            }
            if (songIsPaused) {
                throw runtime_error("Song is already paused.");
            }
            songIsPaused = true;
            cout << "Pausing song: " << currentSong->getTitle() << "\n";

            }
};