#include "PlaybackController.hpp"

#include <algorithm>

void PlaybackController::setPlaylist(const std::vector<Track>& track_vec) {
    player.stop();

    playlist = track_vec;
    playing_index.reset();
}

void PlaybackController::playIndex(std::size_t index) {
    if (index >= playlist.size())
    {
        return;
    }

    playing_index = index;

    player.play(playlist[index]);
}

void PlaybackController::skip() {
    // don't do nothing if there's no playlist yet
    if (playlist.empty())
    {
        return;
    }

    // check if there is a playing index set, if not go to beginning
    if (!playing_index)
    {
        playing_index = 0;
    } else 
    {
        playing_index = (*playing_index + 1) % playlist.size();
    }

    player.play(playlist[*playing_index]);
}

void PlaybackController::rewind() {
    // check if no song has been playing, if not, play the first song in the playlist
    if (!playing_index)
    {
        return;
    }

    // just rewind to start for now

    player.play(playlist[*playing_index]);
}

void PlaybackController::togglePause() {
    player.togglePause();
}

void PlaybackController::stop() {
    player.stop();
    playing_index.reset();
}

bool PlaybackController::hasPlayingTrack() const {
    return playing_index.has_value();
}

const Track* PlaybackController::getPlayingTrack() const {
    if (!playing_index)
    {
        return nullptr;
    }

    return &playlist[*playing_index];
}

std::optional<std::size_t> PlaybackController::getPlayingIndex() const {
    return playing_index;
}

Player::PlaybackState PlaybackController::getPlaybackState() const {
    return player.getPlaybackState();
}

void PlaybackController::update() {
    if (player.consumeFinished())
    {
        skip();
    }
}