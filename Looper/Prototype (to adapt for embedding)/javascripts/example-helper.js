function getBehaviorRef(device, behavior) {
  // Get hash from end of URL or generate a random one.

  var ref = new Firebase('https://looper.firebaseIO.com/users');
  // var hash = window.location.hash.replace(/#/g, '');
  // if (hash) {
  //   ref = ref.child(hash);
  // } else {
  //   ref = ref.push(); // generate unique location.
  //   window.location = window.location + '#' + ref.name(); // add it as a hash to the URL.
  // }

  //ref = ref.push(); // generate unique location.
  var hash = '' + device + '/' + behavior;
  ref = ref.child(hash);
  // window.location = window.location + '#' + ref.name(); // add it as a hash to the URL.

  if (typeof console !== 'undefined')
    console.log('Firebase data: ', ref.toString());

  return ref;
}

// function getExampleRef() {
//   // Get hash from end of URL or generate a random one.

//   var ref = new Firebase('https://pixel-coder.firebaseio.com');
//   var hash = window.location.hash.replace(/#/g, '');
//   if (hash) {
//     ref = ref.child(hash);
//   } else {
//     ref = ref.push(); // generate unique location.
//     window.location = window.location + '#' + ref.name(); // add it as a hash to the URL.
//   }

//   if (typeof console !== 'undefined')
//     console.log('Firebase data: ', ref.toString());

//   return ref;
// }