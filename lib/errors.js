var util = require('util');

const errors = [
  {
    name: 'InternalServerError',
    statusCode: 500,
    message: 'Internal Server Error'
  },
  {
    name: 'UncaughtExceptionError',
    statusCode: 55,
    message: 'Internal Server Error'
  }
];

function customRestifyErrorConstructor(error) {
  return function (message, code) {
    this.constructor.super_.apply(this, arguments);

    this.name = error.name;
    this.message = message || error.message;
    this.statusCode = code || error.statusCode;
    this.body = {
      code: this.name,
      message: this.message
    };

  };
}

function createErrors() {
  errors.forEach(function (err) {
    module.exports[err.name] = new customRestifyErrorConstructor(err);
    util.inherits(module.exports[err.name], Error);
  });
}
module.exports.createErrors = createErrors;