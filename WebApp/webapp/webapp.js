
angular.module('myApp', [
	'ngRoute',
	'mobile-angular-ui'
]).config(function ($routeProvider) {
	$routeProvider.when('/', {
		templateUrl: 'home.html',
		controller: 'Home'
	});
}).controller('Home', function ($scope) {
	var firebaseConfig = {
		apiKey: "AIzaSyCZkXut96ZTsANIFBjxqsxI6eWFg1C5YXM",
		authDomain: "smarthome-116.firebaseapp.com",
		databaseURL: "https://smarthome-116.firebaseio.com",
		projectId: "smarthome-116",
		storageBucket: "smarthome-116.appspot.com",
		messagingSenderId: "1055864949441",
		appId: "1:1055864949441:web:ea1474b982167625cde240"
	};
	// Initialize Firebase
	firebase.initializeApp(firebaseConfig);
	// Get a reference to the database service
	var database = firebase.database();
	var ref = database.ref('/dev/values');
	ref.on('value', function (snapshot) {
		console.log(snapshot.val());
		console.log(snapshot.val().fan);
		$scope.fan_status = snapshot.val().fan | 3;
		$scope.light_status = snapshot.val().light;
		$scope.apt_status = snapshot.val().apt;
		$scope.co2_value = snapshot.val().co2;
		$scope.temp_value = snapshot.val().temp;
		$scope.humi_value = snapshot.val().humi;
	});

});