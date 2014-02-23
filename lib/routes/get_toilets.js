var async = require('async');
var errors = require('../errors.js');

Math.seed = 6;
Math.seededRandom = function (max, min) {
  max = max || 1;
  min = min || 0;

  Math.seed = (Math.seed * 9301 + 49297) % 233280;
  var rnd = Math.seed / 233280;

  var rand = min + rnd * (max - min);
  return rand;
};

const EXTRACTOR_REGEXP = /f(\d+)b(\d+)s(\d+)/;
function parseName(name) {
  return EXTRACTOR_REGEXP.exec(name);
}

const TAGS = ['private', 'handicap'];
var TAG_CACHE = {};
function Shitter(id, name, occupied, lastUpdateTime) {
  var parsedName = parseName(name);
  this.floor = parsedName ? parsedName[1] : name;
  this.bathroom = parsedName ? parsedName[2] : name;
  this.stall = parsedName ? parsedName[3] : name;

  this.id = id;
  this.occupied = occupied ?  true :  false;
  this.lastUpdateTime = lastUpdateTime;

  this.occupyChance = Math.seededRandom(0, 100);

  /** Cheat Codes **/
  if (!TAG_CACHE[id]) {
    TAG_CACHE[id] = [];
    TAGS.forEach(function (tag) {
      if (Math.seededRandom(0, 100) > 60)
        TAG_CACHE[id].push(tag);
    });
  }
  this.tags = TAG_CACHE[id];
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
        transformed.push(new Shitter(stream.id, stream.name, parseInt(stream.value, 10),
          stream.last_value_at));
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