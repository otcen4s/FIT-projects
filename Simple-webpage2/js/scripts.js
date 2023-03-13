function showDiv(param) {
    const e = document.getElementById(param);

    if(!param.localeCompare("show-harvard")){
        document.getElementById("hide-harvard").innerHTML = "Hide";
        hideDiv("show-cambridge");
        hideDiv("show-stanford");
    }
    if(!param.localeCompare("show-cambridge")){
        document.getElementById("hide-cambridge").innerHTML = "Hide";
        hideDiv("show-harvard");
        hideDiv("show-stanford");
    }
    if(!param.localeCompare("show-stanford")){
        document.getElementById("hide-stanford").innerHTML = "Hide";
        hideDiv("show-cambridge");
        hideDiv("show-harvard");
    }

    e.style.display="block";
}

function hideDiv(div){
    const elem = document.getElementById(div);
    elem.style.display = "none";
}

function mobileNav() {
    var e = document.getElementById("hidden-menu");
    if (e.className === "hidden") {
        e.className += " off";
        e.style.display = "flex";
    } else {
        e.className = "hidden";
        e.style.display = "none";
    }
}

function showSubList() {
    var e = document.getElementById("sub-list");
    if (e.className === "hidden") {
        e.className += " off";
        e.style.display = "flex";
    } else {
        e.className = "hidden";
        e.style.display = "none";
    }
}

function changeImageForward(){
    var e = document.getElementById("img-uniq");
    if (e.className === "img-2") {
        e.src = "images/img1.jpg";
        e.className = "img-1";
    }
    else if(e.className === "img-1"){
        e.src = "images/img0.jpg";
        e.className = "img-0";
    }
    else{
        e.src = "images/img2.jpg";
        e.className = "img-2";
    }
}

function changeImageBackward(){
    var e = document.getElementById("img-uniq");
    if (e.className === "img-2") {
        e.src = "images/img0.jpg";
        e.className = "img-0";
    }
    else if(e.className === "img-1"){
        e.src = "images/img2.jpg";
        e.className = "img-2";
    }
    else{
        e.src = "images/img1.jpg";
        e.className = "img-1";
    }
}

function displayPersonalData(param) {
    var e = document.getElementById(param);
    if(param === "relationship"){
        if (e.style.display === "block") {
            e.style.display = "none";
        } else {
            e.style.display = "block";
        }
    }
    else if(param === "group"){
        if (e.style.display === "block") {
            e.style.display = "none";
        } else {
            e.style.display = "block";
        }
    }
    else if(param === "birth"){
        if (e.style.display === "block") {
            e.style.display = "none";
        } else {
            e.style.display = "block";
        }
    }
    else if(param === "address"){
        if (e.style.display === "block") {
            e.style.display = "none";
        } else {
            e.style.display = "block";
        }
    }
    else{
        if (e.style.display === "block") {
            e.style.display = "none";
        } else {
            e.style.display = "block";
        }
    }
}