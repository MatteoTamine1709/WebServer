# WebServer

## Stuff I wanna do

- Tools:

  - [x] Automatically rebuild any .cpp file that has been changed in the `api` folder ([Reference](#hotreload))
    - [x] Currently, if the hot reloader is started and we shhut down the server and open it back up, the hot reloader will not work as it has the incorrect PID.
    - [x] When we start the project with watch mode, the hot reloader is not started
    - [x] Hot reloader should know the set folder for endpoints and not have to be passed as an argument
    - [x] Faster boot up time (compiling the shared library takes a lot of time, make it into it's own thread)
    - [x] Remove infinite loop and use a signal to start/stop the hot reloader
  - [x] A good logger with request time and all (like [morgan](https://www.npmjs.com/package/morgan)) using [spdlog](https://github.com/gabime/spdlog)
    - [ ] Date and time support
    - [x] Log to file
  - [x] Unit testing server with [Catch2](https://github.com/catchorg/Catch2)
  - [ ] Call Javascript functions from C++ [Reference](https://stackoverflow.com/questions/2713289/how-to-execute-javascript-function-in-c)
  - [ ] Dockerize the project (_If needed_)
  - [x] Being able to show the server to other devices on the network (using [ngrok](https://ngrok.com/)?)

- Server:

  - [x] A good config file system using [nlohmann/json](https://github.com/nlohmann/json) ([Reference](#config-file))
    - Add options as the project goes on
  - [x] The ability to write command to the server like `stop` or `status` or `help`
  - [x] [Routing system](#routing-system) like [next.js](https://nextjs.org/docs/routing/introduction)
  - [ ] A way to handle sessions (MAYBE)
  - [ ] Upload of files to the server

- Endpoints:

  - [x] Ability to stream files like video or audio efficiently (not downloading 1GB of data straight on loading) (**Which was the reason I started this project**) ([Reference](https://blog.logrocket.com/build-video-streaming-server-node/))
  - [x] Express interface
    - [x] Middleware support [Reference](https://nextjs.org/docs/advanced-features/middleware)
    - [x] Res and Req from [req](https://expressjs.com/en/5x/api.html#req) and [res](https://expressjs.com/en/5x/api.html#res) <span style="color:red">(Not everything is implemented yet)</span>.
  - [ ] A system to communicate with databases / have modules that every endpoint can access

- Hosting:

  - [ ] A way to host the server on a VPS (using [docker](https://www.docker.com/))

- Optimisation

  - [x] Asynchronous endpoints and read/write
  - [ ] A way to measure the performance of the server using [google benchmark](https://github.com/google/benchmark)
  - [ ] A way to optimise the server to handle a lot of requests (maybe using [libuv](https://libuv.org/))
  - [ ] Reduce memory footprint
  - [ ] Reduce CPU usage

### Routing System

As of right now, we are loading the endpoints from compiled shared library in each folder using a Next like approach. We are able to manage all methods as long as they are defined, otherwise 404 is returned.

- [x] We want to support this type of route aswell
      `pages/blog/first-post.js` → `/blog/first-post`
      `pages/dashboard/settings/username.js` → `/dashboard/settings/username`
- [x] We are missing routing with parameter such as `/user/[id]` or `/user/[id]/[name]`
- [x] We are also missing this type of route `/user?id=1&name=John`
- [x] And we would also like to add a way to handle catch routes like

  - `/user/[...id]`
  - `/user/[...id]/[...otherID]`
  - `/user/[...id]/[...otherID]/name`
  - `/user/[...id]/[...otherID]/name/[...otherName]`

- [x] Also, we would like to handle optional parameters like `/user/[[id]]` or `/user/[[...id]]` (refer to [next.js](https://nextjs.org/docs/routing/dynamic-routes#optional-catch-all-routes) for more info)
- [x] We would like to add a way to handle redirects
- [ ] We would like to make the parameters in the route available in the endpoint like

  - `GET /user/[id]` → `req.params.id`
  - `GET /user/[id]/[name]` → `req.params.id` and `req.params.name`
  - `GET /user/[...id]` → `req.params.id`
  - `GET /user/[...id]/[...otherID]` → `req.params.id` and `req.params.otherID`

- [x] Adding ETag support [Reference](https://stackoverflow.com/questions/4533/http-generating-etag-header)
- Support a maximum of header fields see [here](https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers)

### HotReload

We now have a HotReloader projet which is able to watch for changes in the `api` folder and rebuild the shared library. We are now able to reload the endpoints without having to restart the server.
It works by watching for changes in the `api` folder and then rebuild the shared library. It then sends a signal to the server to reload the endpoints.

### Config File

The goal is to have a config file that can be used to configure the server using a config.json file. We would like to be able to configure the port, the host, the endpoints folder, public folder, is watching for changes, etc.
