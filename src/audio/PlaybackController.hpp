#pragma once

#include <vector>
#include <cstddef>
#include <optional>

#include "player.hpp"
#include "model/track.hpp"

class PlaybackController {
public:
    PlaybackController() = default;
    ~PlaybackController() = default;

    // queue management
    void setPlaylist(const std::vector<Track>& track_vec);

    const std::vector<Track>& getPlaylist() const { return playlist; }

    // volume
    void setVolume(float vol) { player.setVolume(vol); }
    float getVolume() const { return player.getVolume(); }

    // playback commands
    void stop();
    void rewind();
    void skip();
    void togglePause();
    void playIndex(std::size_t index);

    // playback information
    bool hasPlayingTrack() const;
    const Track* getPlayingTrack() const;
    std::optional<std::size_t> getPlayingIndex() const;

    Player::PlaybackState getPlaybackState() const;
    
    // in case a song finishes and needs to go to the next
    void update();
private:
    std::vector<Track> playlist;
    
    Player player;

    std::optional<std::size_t> playing_index;
};