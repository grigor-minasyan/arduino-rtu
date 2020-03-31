// var cur_temp = 15;//{{ cur_temp }};
const update_delay = 2000
function update_temp() {
  $.getJSON($SCRIPT_ROOT + '/_update_cur_temp', function(data) {
    $("#cur_temp").text(data.cur_temp);
    $("#cur_hum").text(data.cur_hum);
  });
  return false;
}
window.setInterval(function(){
  update_temp()
}, update_delay);
