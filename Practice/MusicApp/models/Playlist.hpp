#pragma once

#include <iostream>
#include <vector>
#include <string>
#include "Song.hpp"

using namespace std;

class Playlist{
    private:
        string playlistName;
        vector<Song*> songList;
    public:
        Playlist(string name){
            playlistName=name;
        }

        string getPlaylistName(){
            return playlistName;
        }
        vector<Song*>& getSongs(){
            return songList;
        }
        int getSize(){
            return songList.size();
        }
        void addSongToPlaylist(Song* song){
            if(song==nullptr){
                throw runtime_error("Cannot add null song to playlist.");
            }
            songList.push_back(song);
        }
};