#!/usr/bin/env node

const spotifyZeroconf = require('spotify-zeroconf')
const fs = require('fs');

const main = async () => {
  // Only clientId is required.
  const credentials = await spotifyZeroconf({
      clientId: 'ba1108f53f654010993b14c0b81e4a82', // [Required] Id of your spotify application
      scope: ['user-read-playback-state', 'user-read-currently-playing', 'user-modify-playback-state', 'user-read-recently-played'],// Scopes with a comma separator. Defaults to a list containing every scope. (see here: https://developer.spotify.com/documentation/general/guides/scopes/)
      deviceName: 'RPI Spotify', // Name of the device that appears on the network. Defaults to 'Spotify Zeroconf'.
      // noCache: // If true, will not store the first handshake credentials and will force the device to appear on the network every time the function is called. Defaults to false.
      // authType: // 'token' (default) or 'code'. Code will give you an access code that you can exchange for a token and a refresh_token, but you will need the client_secret. (see here: https://developer.spotify.com/documentation/general/guides/authorization-guide/#2-have-your-application-request-refresh-and-access-tokens-spotify-returns-access-and-refresh-tokens)
  }).catch((err) => {
    console.log(err);
  });

  // Web API token.
  console.log(credentials);
  const data = Object.assign({}, credentials, { access_token: credentials.accessToken });
  try {
    fs.writeFileSync('/opt/sdobox/tmp/spotify.json', JSON.stringify(data));
  } catch (err) {
    console.log(err);
  }
};
main();
