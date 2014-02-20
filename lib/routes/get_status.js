function getStatus(req, res, next) {
  res.status(204);
  res.end();
  return next();
}
module.exports.handler = getStatus;