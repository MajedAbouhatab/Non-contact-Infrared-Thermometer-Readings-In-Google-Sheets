function doPost(e) { 
  var GS = SpreadsheetApp.openById('1Iwqsdbfrcd1pCAFPqflr2evzj9GsYi47RwkkhX6jSs');
  ThisSheet = GS.getActiveSheet();
  // Get all contents
  var json = JSON.parse(e.postData.contents)
  // Row array
  var ThisRecord = [];
  // Report contents
  ThisRecord[0] = json.Temperature;
  ThisRecord[1] = json.ESPChipId;
  // Timestamp
  ThisRecord[2] = ReadableDate(Date.now()); 
  // Save to spreadsheet
  ThisSheet.getRange(ThisSheet.getLastRow() + 1, 1, 1, ThisRecord.length).setValues([ThisRecord]);
}

function ReadableDate(timestamp) {
  var d = new Date(timestamp);
  return d.getFullYear() + "-" + ("00" + (d.getMonth() + 1)).slice(-2) + "-" + ("00" + d.getDate()).slice(-2) + " " +
    ("00" + d.getHours()).slice(-2) + ":" + ("00" + d.getMinutes()).slice(-2) + ":" + ("00" + d.getSeconds()).slice(-2);
}
