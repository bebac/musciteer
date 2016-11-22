Authorizing Spotify Web Access
==============================

Spotify web access authorization is done by redirecting to spotify's account
end point which then redirects back to a callback url. Since Musciteer is meant
to run headless on a local network one probably want's the do the authorization
from a different pc than the one running Musciteer. The problem is that the
callback url must be registered with Spotify and it has to match exactly what is
registered.

For Musciteer the following callback urls are registered

- http://localhost:8214/api/spotify/callback
- http://musciteer:8214/api/spotify/callback

This means that you can register from the same pc that is running the musciteer
service. This is the easiest way. It requires no additional setup and it only
has to be done once as the access token is persisted.

To do it from another pc, some setup is required. The pc running Musciteer must be
reachable by the hostname musciteer. It can be achieved by adding musciteer to
the /etc/hosts file. Edit /etc/hosts and add something like this

    192.168.1.100 musciteer

Replace the 192.168.1.100 with the IP of the pc running musciteer.

Test you can ping it

    ping musciteer

If that works, you should be able to authorize spotify from the web settings
interface.
