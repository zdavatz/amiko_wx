/*
 Copyright (c) 2017 Max Lungarella <cybrmx@gmail.com>
 Created on 28/04/2017.
 All main JS callbacks
 */

document.ondrop = function(ev) {
    ev.preventDefault();
    event.stopPropagation();
    var theFile = null;
    if (ev.dataTransfer.items) {
        // Use DataTransferItemList interface to access the file(s)
        for (var i = 0; i < ev.dataTransfer.items.length; i++) {
            // If dropped items aren't files, reject them
            if (ev.dataTransfer.items[i].kind === 'file') {
                var file = ev.dataTransfer.items[i].getAsFile();
                theFile = file;
                break;
            }
        }
    } else {
        if (ev.dataTransfer.files.length > 0) {
            theFile = ev.dataTransfer.files[0];
        }
    }
    if (theFile) {
        var reader = new FileReader();
        reader.readAsText(theFile, "UTF-8");
        reader.onload = function (evt) {
            // file content is too big we cannot pass it via title
            location.href = "http://file-drop/" + theFile.name + "/" + evt.target.result;
        }
    }
}

document.ondragover = function(ev) {
    ev.preventDefault();
    event.stopPropagation();
}

/**
 */
function displayFachinfo(ean, anchor) {
    try {
        if (anchor == 'undefined')
            anchor = '';

        var payload = ["main_cb", "display_fachinfo", ean, anchor];

        // Change the title to something that allows you to pass information from JS to C++
        document.title = JSON.stringify(payload);
    } catch (e) {
        alert(e);
    }
}

function highlightText(node, text) {
    if (node instanceof Text) {
        var splitted = node.data.split(text);
        if (splitted.length === 1) {
            // Not found, no replace
            return null;
        }
        // Create a new element with text highlighted
        var wrapper = document.createElement('span');
        for (var i = 0; i < splitted.length; i++) {
            var thisText = document.createTextNode(splitted[i]);
            wrapper.appendChild(thisText);
            if (i !== splitted.length - 1) {
                var thisSpan = document.createElement('span');
                thisSpan.innerText = text;
                thisSpan.style.backgroundColor = getComputedStyle(document.documentElement).getPropertyValue('--text-color-highlight');
                thisSpan.className = 'mark';
                wrapper.appendChild(thisSpan);
            }
        }
        return wrapper;
    }
    var nodeName = node.nodeName.toLowerCase();
    if (nodeName === 'script' || nodeName === 'style') {
        return null;
    }
    var nodes = node.childNodes;
    for (var i = 0; i < nodes.length; i++) {
        var thisNode = nodes[i];
        var newNode = highlightText(thisNode, text);
        if (newNode !== null) {
            node.replaceChild(newNode, thisNode);
        }
    }
    return null;
}

/**
 * Identifies the anchor's id and scrolls to the first mark tag.
 * Javascript is brilliant :-)
 */
function moveToHighlight(anchor) {
    if (typeof anchor !== 'undefined') {
        try {
            var elem = document.getElementById(anchor);
            if (elem !== null) {
                var marks = elem.getElementsByClassName('mark')
                if (marks.length > 0) {
                    marks[0].scrollIntoView(true);
                }
            }
        } catch(e) {
            alert(e);
        }
    }
}

// type: [{ count: 1, textNode: <text node> }]
var matchedTextNodes = [];
var totalMatchCount = 0;
var currentSearchTerm = '';
// type: -1 means not highlighted, otherwise selected index
var currentHighlighedIndex = -1;
// type: function(): void | null
var restoreCurrentHighlighedText = null;

function searchText(text) {
    presearchTextInPage(text);
    highlightSearchResult(0);
}

function presearchTextInPage(text, node) {
    resetSearchInPage();
    if (node === undefined) {
        node = document.body;
    }
    currentSearchTerm = text;
    matchedTextNodes = findTextNodesWithText(text, node);
    totalMatchCount = matchedTextNodes.reduce(function(acc, curr){ return acc + curr.count }, 0);
}

function resetSearchInPage() {
    if (restoreCurrentHighlighedText !== null) {
        restoreCurrentHighlighedText();
        restoreCurrentHighlighedText = null;
    }
    currentSearchTerm = '';
    matchedTextNodes = [];
    totalMatchCount = 0;
    currentHighlighedIndex = -1;
}

function highlightNextResult() {
    if (currentHighlighedIndex >= totalMatchCount - 1) {
        highlightSearchResult(0);
    } else {
        highlightSearchResult(currentHighlighedIndex + 1);
    }
    
}

function highlightPreviousResult() {
    if (currentHighlighedIndex <= 0) {
        highlightSearchResult(totalMatchCount - 1);
    } else {
        highlightSearchResult(currentHighlighedIndex - 1);
    }
}

function highlightSearchResult(index) {
    if (index >= totalMatchCount) return;
    currentHighlighedIndex = index;
    if (restoreCurrentHighlighedText !== null) {
        restoreCurrentHighlighedText();
        restoreCurrentHighlighedText = null;
    }
    var count = 0;
    for (var i = 0; i < matchedTextNodes.length; i++) {
        var match = matchedTextNodes[i];
        if (count + match.count > index) {
            var subIndex = index - count; // The x match of this string
            var node = match.textNode;
            var regexp = new RegExp(escapeRegExp(currentSearchTerm), 'gi');
            var match = null;
            var matchIndex = 0;
            var before, highlightedText, after;
            while ((match = regexp.exec(node.data)) != null) {
              if (matchIndex === subIndex) {
                before = node.data.slice(0, match.index);
                highlightedText = match[0];
                after = node.data.slice(match.index + highlightedText.length);
                break;
              }
              matchIndex++;
            }
            // Create a new element with text highlighted
            var wrapper = document.createElement('span');
            wrapper.appendChild(document.createTextNode(before));
            var highlightedSpan = document.createElement('span');
            highlightedSpan.innerText = highlightedText;
            highlightedSpan.style.backgroundColor = getComputedStyle(document.documentElement).getPropertyValue('--text-color-highlight') || 'yellow';
            highlightedSpan.className = 'mark';
            wrapper.appendChild(highlightedSpan);
            wrapper.appendChild(document.createTextNode(after));
            // node.replaceWith(wrapper);
            node.parentNode.replaceChild(wrapper, node);
            restoreCurrentHighlighedText = function() {
                wrapper.parentNode.replaceChild(node, wrapper);
            }
            wrapper.scrollIntoView(true);
            break;
        }
        count += match.count;
    }
    var payload = ["main_cb", "in_page_search", (index + 1) + "/" + totalMatchCount];

    // Change the title to something that allows you to pass information from JS to C++
    document.title = JSON.stringify(payload);
}

// pure
function findTextNodesWithText(text, node) {
    if (node instanceof Text) {
        var re = new RegExp(escapeRegExp(text), 'gi');
        var match = null;
        var count = 0;
        while((match = re.exec(node.data)) !== null) {
          count++;
        }
        if (count === 0) {
            // Not found, no replace
            return [];
        }
        return [{
            count: count,
            textNode: node
        }];
    }
    var nodeName = node.nodeName.toLowerCase();
    if (nodeName === 'script' || nodeName === 'style') {
        return [];
    }
    var nodes = node.childNodes;
    var results = [];
    for (var i = 0; i < nodes.length; i++) {
        var thisNode = nodes[i];
        var subResult = findTextNodesWithText(text, thisNode);
        results = results.concat(subResult);
    }
    return results;
}

function escapeRegExp(string) {
    return string.replace(/[.*+?^${}()|[\]\\]/g, '\\$&'); // $& means the whole matched string
}
