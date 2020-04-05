// Get the Sidebar
var sidebar = document.getElementById("sidebar");

// Get the DIV with overlay effect
var overlayBg = document.getElementById("overlay");

// Toggle between showing and hiding the sidebar, and add overlay effect
function open_sidebar() {
  if (sidebar.style.display === 'block') {
    sidebar.style.display = 'none';
    overlayBg.style.display = "none";
  } else {
    sidebar.style.display = 'block';
    overlayBg.style.display = "block";
  }
}

// Close the sidebar with the close button
function close_sidebar() {
  sidebar.style.display = "none";
  overlayBg.style.display = "none";
}