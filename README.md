# WebServer

## Stuff I wanna do

- Tools:

  - [x] Automatically rebuild any .cpp file that has been changed in the `endpoints` folder ([Reference](#hotreload))
    - [ ] Currently, if the hot reloader is started and we shhut down the server and open it back up, the hot reloader will not work as it has the incorrect PID.
    - [ ] When we start the project with watch mode, the hot reloader is not started
    - [ ] Hot reloader should know the set folder for endpoints and not have to be passed as an argument
  - [x] A good logger with request time and all (like [morgan](https://www.npmjs.com/package/morgan)) (maybe using [spdlog](https://github.com/gabime/spdlog))
    - [ ] Date and time support
    - [x] Log to file
  - [ ] Testing server with [Catch2](https://github.com/catchorg/Catch2)
  - [ ] Dockerize the project (_If needed_)

- Server:

  - [x] A good config file system (maybe using [nlohmann/json](https://github.com/nlohmann/json)) ([Reference](#config-file))
    - [ ] Add options as the project goes on
  - [ ] [Routing system](#routing-system) like [next.js](https://nextjs.org/docs/routing/introduction)
  - [ ] A way to handle sessions (MAYBE)

- Endpoints:

  - [ ] Ability to stream files like video or audio efficiently (not downloading 1GB of data straight on loading) (**Which was the reason I started this project**) ([Reference](https://blog.logrocket.com/build-video-streaming-server-node/))
  - [ ] A system to communicate with databases / have modules that every endpoint can access

- Hosting:

  - [ ] A way to host the server on a VPS (maybe using [docker](https://www.docker.com/))

- Optimisation
  - [ ] A way to optimise the server to handle a lot of requests (maybe using [libuv](https://libuv.org/))
  - [ ] Reduce memmory footprint
  - [ ] Reduce CPU usage

### Routing System

As of right now, we are loading the endpoints from compiled shared library in each folder using a Next like approach. We are able to manage all methods as long as they are defined, otherwise 404 is returned.

[x] We want to support this type of route aswell
`pages/blog/first-post.js` → `/blog/first-post`
`pages/dashboard/settings/username.js` → `/dashboard/settings/username`
[ ] We are missing routing with parameter such as `/user/:id` or `/user/:id/:name`
[ ] We are also missing this type of route `/user?id=1`
[ ] And we would also like to add a way to handle all routes like `*` or `**`
[ ] Also, we would like to handle optional parameters like `/user/:id?` (refer to [next.js](https://nextjs.org/docs/routing/dynamic-routes#optional-catch-all-routes) for more info)

### HotReload

We now have a HotReloader projet which is able to watch for changes in the `endpoints` folder and rebuild the shared library. We are now able to reload the endpoints without having to restart the server.
It works by watching for changes in the `endpoints` folder and then rebuild the shared library. It then sends a signal to the server to reload the endpoints.

### Config File

The goal is to have a config file that can be used to configure the server using a config.json file. We would like to be able to configure the port, the host, the endpoints folder, public folder, is watching for changes, etc.
