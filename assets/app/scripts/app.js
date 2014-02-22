angular.module('app', [])

.controller('controller', function($scope) {
  $scope.loos = [
    {
      floor: '14th Floor',
      bathroom: 'front', 
      stall: 1, 
      cleanliness: 1, 
      occupy_chance: 20, 
      tags: ['handicap', 'private']
    },
    {
      floor: '13th Floor',
      bathroom: 'front', 
      stall: 2, 
      cleanliness: 0, 
      occupy_chance: 80, 
      tags: []
    }
  ];
});