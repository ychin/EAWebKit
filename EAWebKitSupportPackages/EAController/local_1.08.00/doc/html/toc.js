// Note that menu elements cannot have trailing commas after the last item in the
// array otherwise an extra, undefined, element is appended. In IE this manifests
// as the sidebar not being rendered.
var menuElements = [
    [
        "EAController", [
            ["Home", "index.html"],
            ["Change Log", "changelog.html"],
            ["Email EAController Support", "mailto:EATechCoreSupport@ea.com"]
        ]
    ]
];

function buildCategory(categoryData, linkPrefix) {
    var categoryName = categoryData[0];
    var categoryContents = categoryData[1];

    var categoryText = "<h3>" + categoryName + "</h3>";
    categoryText += "<ul>";

    for (var i = 0; i < categoryContents.length; ++i) {
        var linkText = categoryContents[i][0];
        var linkURL = categoryContents[i][1];
        categoryText += "<li>";

        if (linkURL.search("mailto") == -1) {
            linkURL = linkPrefix + linkURL;
        }

        categoryText += linkText.link(linkURL);
        categoryText += "</li>";
    }

    categoryText += "</ul>";
    return categoryText;
}

function buildMenu(linkPrefix) {
    var menu = "";

    for (var i = 0; i < menuElements.length; ++i) {
        menu += buildCategory(menuElements[i], linkPrefix);
    }

    return menu;
}

function loadMenu(linkPrefix) {
    var menuBar = document.getElementById("menubar");
    menuBar.innerHTML = buildMenu(linkPrefix);

    document.getElementById("header").innerHTML = "<img src=\"" + linkPrefix + "img/logo.png\"/>";
    document.getElementById("footer").innerHTML = "<center><h4>Copyright (c) Electronic Arts.</h4></center>";
}
