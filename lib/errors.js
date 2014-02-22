var util = require('util');
var find = require('mout/array/find');

const errors = [
  // General Purpose Errors
  {
    name: 'BadRequestError',
    statusCode: 400,
    message: 'Bad Request Error'
  },
  {
    name: 'UnauthorizedError',
    statusCode: 401,
    message: 'Unauthorized Error'
  },
  {
    name: 'ForbiddenError',
    statusCode: 403,
    message: 'Forbidden Error'
  },
  {
    name: 'NotFoundError',
    statusCode: 404,
    message: 'Not Found Error'
  },
  {
    name: 'MethodNotAllowed',
    statusCode: 405,
    message: 'Not Found Error'
  },
  {
    name: 'UnprocessableEntityError',
    statusCode: 422,
    message: 'Unprocessable Entity Error'
  },
  {
    name: 'InternalServerError',
    statusCode: 500,
    message: 'Internal Server Error'
  },
  {
    name: 'BadGatewayError',
    statusCode: 502,
    message: 'Bad Gateway Error'
  },
  {
    name: 'ServiceUnavailableError',
    statusCode: 503,
    message: 'Service Unavailable Error'
  },
  {
    name: 'GatewayTimeoutError',
    statusCode: 504,
    message: 'Gateway Timeout Error'
  },
  // Begin Custom Errors
  {
    name: 'UncaughtExceptionError',
    statusCode: 500,
    message: 'Internal Server Error'
  },
  {
    name: 'UnknownError',
    message: 'Unknown Error'
  }
];

function getBestError(statusCode, message) {
  var bestMatch = find(errors, {statusCode: statusCode}) ||
    {name: 'UnknownError', statusCode: statusCode, message: message};

  return new module.exports[bestMatch.name](
    message || bestMatch.message,
    bestMatch.statusCode
  );
}
module.exports.getBestError = getBestError;

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