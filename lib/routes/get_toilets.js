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

const EXTRACTOR = /f(\d+)b(\d+)s(\d+)/;
function parseName(name) {
  return EXTRACTOR.exec(name);
}

const TAGS = ['private', 'handicap'];
const CHEATER_MAP = {};
function Shitter(id, name, occupied, lastUpdateTime) {
  this.id = id;
  var parsedName = parseName(name);
  this.floor = parsedName ? parsedName[1] : name;
  this.bathroom = parsedName ? parsedName[2] : name;
  this.stall = parsedName ? parsedName[3] : name;
  this.occupied = occupied ?  true :  false;

  /** Cheat Codes **/
  this.occupyChance = Math.seededRandom(0, 100);

  if (!CHEATER_MAP[id]) {
    CHEATER_MAP[id] = [];
    TAGS.forEach(function (tag) {
      if (Math.seededRandom(0, 100) > 60)
        CHEATER_MAP[id].push(tag);
    });
  }
  this.tags = CHEATER_MAP[id];

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