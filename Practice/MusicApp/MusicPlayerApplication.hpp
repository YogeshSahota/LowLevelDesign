#pragma once

#include "MusicPlayerFacade.hpp"
#include "managers/PlaylistManager.hpp"

using namespace std;

class MusicPlayerApplication{
    private:
        static MusicPlayerApplication* instance;
        vector<Song*> songLibrary;

        MusicPlayerApplication(){}

    public:
        static MusicPlayerApplication* getInstance(){
            if(!instance){
                instance = new MusicPlayerApplication();
            }
            return instance;
        }

        void createSongInLibrary(const string &title,const string &artist,const string &path){
            Song* song = new Song(title,artist,path);
            songLibrary.push_back(song);
        }

        Song* findSongByTitle(const string &title){
            for(auto song:songLibrary){
                if(song->getTitle()==title){
                    return song;
                }
            }
            return nullptr;
        }

        void createPlaylist(const string &name){
            PlaylistManager::getInstance()->createPlaylist(name);
        }

        void addSongToPlaylist(const string& playlistName, const string& songTitle){
            Song* song = findSongByTitle(songTitle);

            if(!song){
                throw runtime_error("Song \"" + songTitle + "\" not found.");
            }

            PlaylistManager::getInstance()->addSongToPlaylist(playlistName,song);
        }

        void connectAudioDevice(DeviceType deviceType){
            MusicPlayerFacade::getInstance()->connectDevice(deviceType);
        }

        void selectPlayStrategy(PlayStrategyType strategyType){
            MusicPlayerFacade::getInstance()->setPlayStrategy(strategyType);
        }

        void loadPlaylist(const string &playlistName){
            MusicPlayerFacade::getInstance()->loadPlaylist(playlistName);
        }

        void playSingleSong(const string& songTitle){
            Song* song = findSongByTitle(songTitle);

            if(!song){
                throw runtime_error("Song \"" + songTitle + "\" not found.");
            }

            MusicPlayerFacade::getInstance()->playSong(song);
        }

        void pauseCurrentSong(const string& songTitle){
            Song* song = findSongByTitle(songTitle);

            if(!song){
                throw runtime_error("Song \"" + songTitle + "\" not found.");
            }

            MusicPlayerFacade::getInstance()->pauseSong(song);
        }

        void playAllTracksInPlaylist(){
            MusicPlayerFacade::getInstance()->playAllTracks();
        }

        void playPreviousTrackInPlaylist(){
            MusicPlayerFacade::getInstance()->playPreviousTrack();
        }

        void queueSongNext(const string& songTitle) {
            Song* song = findSongByTitle(songTitle);
            if (!song) {
                throw runtime_error("Song \"" + songTitle + "\" not found.");
            }
            MusicPlayerFacade::getInstance()->enqueueNext(song);
        }
};

MusicPlayerApplication* MusicPlayerApplication::instance = nullptr;