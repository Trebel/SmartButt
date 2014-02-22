var async = require('async');
var errors = require('../errors.js');

function getRandomArbitrary(min, max) {
  return Math.random() * (max - min) + min;
}

const EXTRACTOR = /f(\d+)b(\d+)s(\d+)/;
function parseName(name) {
  return EXTRACTOR.exec(name);
}
console.dir(parseName('f12b23s12312'));

const TAGS = ['handicap', 'private'];
function Shitter(id, name, occupied, lastUpdateTime) {
  var self = this;

  this.id = id;
  var parsedName = parseName(name);
  this.floor = parsedName ? parsedName[1] : name;
  this.bathroom = parsedName ? parsedName[2] : name;
  this.stall = parsedName ? parsedName[3] : name;
  this.occupied = occupied ?  true :  false;
  this.occupyChance = getRandomArbitrary(0, 100);
  this.tags = [];
  TAGS.forEach(function (tag) {
    if (getRandomArbitrary(0, 100) > 50)
      self.tags.push(tag);
  });
  this.lastUpdateTime = lastUpdateTime;
}

function getShitters(req, res, next) {
  async.auto({
    fetchStreams: function (callback) {
      req.m2x.feeds.streams('62b83d2c715f6ddcc38056a0f0023d0f', function (data, err) {
        if (err instanceof Error) return callback(err);

        return callback(null, data);
      });
    },
    transformed: ['fetchStreams', function (callback, results) {
      var streams = results.fetchStreams.streams;

      var transformed = [];
      streams.forEach(function (stream) {
        transformed.push(new Shitter(stream.id, stream.name, stream.value, stream.last_value_at));
      });
      callback(null, transformed);
    }]
  }, function (err, results) {
    if (err instanceof Error)
      return next(errors.getBestError(err.statusCode, err.message));

    res.json(200, results.transformed);
    return next();
  });
}
module.exports.handler = getShitters;