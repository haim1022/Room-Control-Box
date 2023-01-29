var cacheName = 'SmartRoomApp';
var filesToCache = [
  './',
  './index.html',
  './api_settings.html',
  './style.css',
  './main.js'
];
 
/* Start the service worker and cache files in filesToCache */
self.addEventListener('install', function(e) {
  e.waitUntil(
    caches.open(cacheName).then(function(cache) {
      return cache.addAll(filesToCache); 
    })
  );
});
 
/* Serve cached content when offline */
self.addEventListener('fetch', function(e) {
  e.respondWith(
    caches.match(e.request).then(function(response) {
      return response || fetch(e.request);
    })
  );
});