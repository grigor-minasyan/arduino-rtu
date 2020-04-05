// var cur_temp = 15;//{{ cur_temp }};
let update_delay = 4000;
let RTU_id = 2;
let RTU2_id = 3;

function to_f(c) {
  return 1.8*c+32;
}

function validate_form(ipaddress, port, id){
  if (!(/^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$/.test(ipaddress))){
    alert("You have entered an invalid IP address!");
    return (false);
  }
  if (isNaN(port) || port < 1 || port % 1 != 0) {
    alert("Port number is invalid")
    return false;
  }
  if (isNaN(id) || id < 1 || id % 1 != 0) {
    alert("ID number is invalid")
    return false;
  }
  return true;
}

function submit_rtu_data() {
  let ip_address = document.getElementById("ip_address").value;
  var port = document.getElementById("port").value;
  var device_id = document.getElementById("device_id").value;
  if (validate_form(ip_address, port, device_id)) {
    alert("\nip_address: " + ip_address + "\nport: " + port + "\ndevice_id: " + device_id + "\nForm Submitted Successfully......");
    $('input[type="text"], textarea').val('');
  }
}

var RTU_obj;
function update_temp(id) {
  $.getJSON($SCRIPT_ROOT + '/_update_cur_temp/' + id.toString(), function(data) {
    // $("#cur_temp").text(data.cur_temp);
    // $("#cur_hum").text(data.cur_hum);
    RTU_obj = data;
    RTU_obj[4].sort(function(item2, item1){
      if (item1[0] != item2[0]) return item1[0] - item2[0]
      else if (item1[1] != item2[1]) return item1[1] - item2[1];
      else if (item1[2] != item2[2]) return item1[2] - item2[2];
      else if (item1[3] != item2[3]) return item1[3] - item2[3];
      else if (item1[4] != item2[4]) return item1[4] - item2[4];
      else if (item1[5] != item2[5]) return item1[5] - item2[5];
      else return 0;
    });
    str = "";
    for (let i = 0; i < Object.keys(RTU_obj[4]).length; i++) {
      str = str + "<tr><td>" +
      RTU_obj[4][i][0].toString() + "/" +
      RTU_obj[4][i][1].toString() + "/" +
      RTU_obj[4][i][2].toString() + "</td><td>" +
      RTU_obj[4][i][3].toString() + ":" +
      RTU_obj[4][i][4].toString() + ":" +
      RTU_obj[4][i][5].toString() + "</td><td>" +
      RTU_obj[4][i][6].toString() + "C " +
      RTU_obj[4][i][7].toString() + "%" +"</td></tr>";
    }
    $("#history"+id.toString()).html(str);
    $("#temp"+id.toString()).html(to_f(RTU_obj[3][6]).toFixed(1));
    $("#hum"+id.toString()).html(RTU_obj[3][7]);
    if(RTU_obj[2] == 3) {
      //major over
      $("#alarm"+id.toString()).html("Alarm MJ OVER!");
    } else if (RTU_obj[2] == 2) {
      //minor over
      $("#alarm"+id.toString()).html("Alarm MN OVER!");
    } else if (RTU_obj[2] == 4) {
      // minor under
      $("#alarm"+id.toString()).html("Alarm MN UNDER!");
    } else if (RTU_obj[2] == 12) {
      //major under
      $("#alarm"+id.toString()).html("Alarm MJ UNDER!");
    } else {
      $("#alarm"+id.toString()).html("No alarm, everything clear");
    }
  });
  return false;
}
window.setInterval(function(){
  update_temp(RTU_id);
  update_temp(RTU2_id);
}, update_delay);

// window.onload = function() {
//   setTimeout(function(){update_temp(RTU_id); }, 1000);
//   setTimeout(function(){update_temp(RTU2_id); }, 2000);
//   setTimeout(function(){update_temp(RTU_id); }, 3000);
//   setTimeout(function(){update_temp(RTU2_id); }, 4000);
// };
