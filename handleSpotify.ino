void handleSpotify() //Recieves argument for play device, playlist and action, auth token is handled in the backend
{
  handleRoot();
  const char * refreshToken = NULL;
  char * requestedDeviceID = "";  
  if(server.hasArg("code"))
    refreshToken = spotify.requestAccessTokens(server.arg("code").c_str(), spotifyCallbackURI);
  if(server.hasArg("device") && server.arg("device") != "")
  {
    SpotifyDevice spotifyDevice = spotify.getDeviceByName(server.arg("device").c_str(), SPOTIFY_COUNTRY_CODE);
    if(!spotifyDevice.deviceFound)
      return;
    requestedDeviceID = spotifyDevice.id;
  }
  if(server.hasArg("shuffle"))
    spotify.toggleShuffle(constrain(strtoul(server.arg("shuffle").c_str(), NULL, 10), 0, 1), requestedDeviceID);
  if(server.hasArg("repeat"))
  {
    byte repeatRequest = constrain(strtoul(server.arg("shuffle").c_str(), NULL, 10), 0, 2);
    if(repeatRequest == 0)
      spotify.setRepeatMode(repeat_off, requestedDeviceID);
    else if(repeatRequest == 1)
      spotify.setRepeatMode(repeat_context, requestedDeviceID);
    else if(repeatRequest == 2)
      spotify.setRepeatMode(repeat_track, requestedDeviceID);
  }
  if(server.hasArg("playlist"))
  {
    char bodyRequest[200];
    const char * requestedPlaylist = server.arg("playlist").c_str();
    unsigned int playlistTrackNum = 0; //Default is the first song in the playlist
    
    if(server.hasArg("position"))
       playlistTrackNum = strtoul(server.arg("position").c_str(), NULL, 10);
    sprintf(bodyRequest, "{\"context_uri\" : \"spotify:playlist:%s\", \"offset\": {\"position\": %d}}", requestedPlaylist, playlistTrackNum);
    spotify.playAdvanced(bodyRequest, requestedDeviceID);
  }
  if(server.hasArg("action"))
  {
    byte spotifyAction = strtoul(server.arg("action").c_str(), NULL, 10);

    if(spotifyAction == SPOTIFY_ACTION_PLAY)
      spotify.play(requestedDeviceID);
    if(spotifyAction == SPOTIFY_ACTION_PAUSE)
      spotify.pause(requestedDeviceID);
    if(spotifyAction == SPOTIFY_ACTION_NEXT)
      spotify.nextTrack(requestedDeviceID);
    if(spotifyAction == SPOTIFY_ACTION_PREVIOUS)
      spotify.previousTrack(requestedDeviceID);
  }
}
