# Restify Skeleton

A minimalist skeleton for bring up a restify server in minimal time.

Use `npm install` to install dependencies, and `npm install -g grunt-cli` to install grunt-cli globally.

To start the server, utilize `grunt server`. By default, the server configuration provided at `./conf/server.json` will be utilized.

In order to utilize an alternative configuration, set environment variable `SERVERCONF` as the desired location of the configuration. For example, `SERVERCONF=~/config.json grunt server`.

## Supported Grunt Commands
 * [default] - run jshint and mocha tests
 * [server] - run jshint, mocha test, and start the server
 * [test] - run mocha tests
 * [lint] - run jshint