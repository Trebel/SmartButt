# SmartButt API

A simple web app and REST API implementation which uses sensor data stored in AT&T's m2x.

## Quick Start
```bash
git clone https://github.com/Trebel/SmartButt.git
cd SmartButt
npm install
grunt server
```

```html
http://localhost:5000/index.html
http://localhost:5000/api/toilets
```

## Usage
Use `npm install` to install dependencies, and `npm install -g grunt-cli` to install grunt-cli globally.

To start the server, utilize `grunt server`. By default, the server configuration provided at `./conf/server.json` will be utilized.

In order to utilize an alternative configuration, set environment variable `SERVERCONF` as the desired location of the configuration. For example, `SERVERCONF=~/config.json grunt server`.

## Supported Grunt Commands
 * `grunt` - run jshint and mocha tests
 * `grunt server` - run jshint, mocha test, and start the server
 * `grunt test` - run mocha tests
 * `grunt lint` - run jshint