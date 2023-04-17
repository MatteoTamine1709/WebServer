# WebServer

## Stuff I wanna do

- Tools:

  - [x] Automatically rebuild any .cpp file that has been changed in the `endpoints` folder ([Reference](#hotreload))
  - [ ] A good logger with request time and all (like [morgan](https://www.npmjs.com/package/morgan)) (maybe using [spdlog](https://github.com/gabime/spdlog))
  - [ ] Log to file
  - [ ] Dockerize the project (_If needed_)

- Server:

  - [ ] A good config file (maybe using [nlohmann/json](https://github.com/nlohmann/json)) (_If needed_)
  - [ ] [Routing system](#routing-system) like [next.js](https://nextjs.org/docs/routing/introduction)
  - [ ] A way to handle sessions (MAYBE)

- Endpoints:

  - [ ] Ability to stream files like video or audio efficiently (not downloading 1GB of data straight on loading) (**Which was the reason I started this project**) ([Reference](https://blog.logrocket.com/build-video-streaming-server-node/))
  - [ ] A system to communicate with databases / have modules that every endpoint can access

- Hosting:
  - [ ] A way to host the server on a VPS (maybe using [docker](https://www.docker.com/))

### Routing System

As of right now, we are loading the endpoints from compiled shared library in each folder using a Next like approach. We are able to manage all methods as long as they are defined, otherwise 404 is returned.

We are missing routing with parameter such as `/user/:id` or `/user/:id/:name`
We are also missing this type of route `/user?id=1`
And we would also like to add a way to handle all routes like `*` or `**`
Also, we would like to handle optional parameters like `/user/:id?` (refer to [next.js](https://nextjs.org/docs/routing/dynamic-routes#optional-catch-all-routes) for more info)

### HotReload

We now have a HotReloader projet which is able to watch for changes in the `endpoints` folder and rebuild the shared library. We are now able to reload the endpoints without having to restart the server.
