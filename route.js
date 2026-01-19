// City Coordination Graph (x, y in percentages)
const cityCoords = {
    "Karachi": { x: 20, y: 85 },
    "Hyderabad": { x: 28, y: 78 },
    "Sukkur": { x: 35, y: 65 },
    "Quetta": { x: 15, y: 55 },
    "Multan": { x: 50, y: 50 },
    "Lahore": { x: 70, y: 40 },
    "Islamabad": { x: 60, y: 20 },
    "Peshawar": { x: 50, y: 15 }
};

// Adjacency List
const connections = [
    ["Karachi", "Hyderabad"],
    ["Hyderabad", "Sukkur"],
    ["Sukkur", "Multan"],
    ["Sukkur", "Quetta"],
    ["Quetta", "Multan"],
    ["Multan", "Lahore"],
    ["Lahore", "Islamabad"],
    ["Multan", "Islamabad"],
    ["Islamabad", "Peshawar"]
];

const mapContainer = document.getElementById("mapContainer");
const svgLayer = document.getElementById("mapSvg");
const nodesLayer = document.getElementById("nodesLayer");
const marker = document.getElementById("parcelMarker");
const statusLabel = document.getElementById("routeStatus");

let simulationInterval = null;

// UI Toggles
function toggleMode() {
    const manual = document.getElementById("modeManual").checked;
    document.getElementById("manualControls").style.display = manual ? "grid" : "none";
    document.getElementById("idControls").style.display = manual ? "none" : "grid";

    // Reset Map
    renderStaticMap();
    statusLabel.innerText = "Ready.";
    marker.style.display = "none";
}

function initAdvancedMap() {
    renderStaticMap();

    // Check if ID passed via URL (e.g., from Inventory page)
    const urlParams = new URLSearchParams(window.location.search);
    const idFromUrl = urlParams.get('id');
    if (idFromUrl) {
        document.getElementById("modeID").checked = true;
        toggleMode();
        document.getElementById("trackIdInput").value = idFromUrl;
        startIdSimulation();
    }
}

function renderStaticMap() {
    nodesLayer.innerHTML = "";
    svgLayer.innerHTML = "";

    connections.forEach(edge => {
        const c1 = cityCoords[edge[0]];
        const c2 = cityCoords[edge[1]];
        drawSVGLine(c1, c2, "bg-path");
    });

    Object.keys(cityCoords).forEach(city => {
        const coords = cityCoords[city];
        const node = document.createElement("div");
        node.className = "map-node";
        node.style.left = coords.x + "%";
        node.style.top = coords.y + "%";
        node.title = city;
        node.id = `node-${city}`;

        const label = document.createElement("div");
        label.className = "node-label";
        label.textContent = city;

        node.appendChild(label);
        nodesLayer.appendChild(node);
    });
}

function getSvgPoints(c1, c2) {
    const w = mapContainer.clientWidth;
    const h = mapContainer.clientHeight;
    return {
        x1: (c1.x / 100) * w,
        y1: (c1.y / 100) * h,
        x2: (c2.x / 100) * w,
        y2: (c2.y / 100) * h
    };
}

function drawSVGLine(c1, c2, className) {
    const p = getSvgPoints(c1, c2);
    const pathStr = `M ${p.x1} ${p.y1} L ${p.x2} ${p.y2}`;
    const path = document.createElementNS("http://www.w3.org/2000/svg", "path");
    path.setAttribute("d", pathStr);
    path.setAttribute("class", className);
    svgLayer.appendChild(path);
    return path;
}

// Logic: BFS Pathfinding
function findPath(start, end) {
    let queue = [[start]];
    let visited = new Set();
    visited.add(start);

    while (queue.length > 0) {
        let path = queue.shift();
        let node = path[path.length - 1];

        if (node === end) return path;

        let neighbors = [];
        connections.forEach(edge => {
            if (edge[0] === node) neighbors.push(edge[1]);
            if (edge[1] === node) neighbors.push(edge[0]);
        });

        neighbors.forEach(neighbor => {
            if (!visited.has(neighbor)) {
                visited.add(neighbor);
                let newPath = [...path, neighbor];
                queue.push(newPath);
            }
        });
    }
    return null;
}

// SIMULATION: Manual
function startAdvancedSimulation() {
    const startCity = document.getElementById("startCity").value;
    const endCity = document.getElementById("endCity").value;
    runSimulation(startCity, endCity);
}

// SIMULATION: By ID
function startIdSimulation() {
    const id = document.getElementById("trackIdInput").value.trim().toUpperCase();
    if (!id) { alert("Please enter an ID"); return; }

    // Access the 'SwiftExApp' from parcel.js (it must be loaded)
    const parcel = SwiftExApp.getParcel(id);

    if (!parcel) {
        statusLabel.innerHTML = `<span style="color:red">Parcel ID ${id} not found in database!</span>`;
        return;
    }

    statusLabel.innerHTML = `Found Parcel <b>${id}</b>. tracking from ${parcel.sender} to ${parcel.receiver}...`;

    // small delay to read status
    setTimeout(() => {
        runSimulation(parcel.sender, parcel.receiver);
    }, 1000);
}

// CORE RUNNER
function runSimulation(start, end) {
    if (start === end) {
        alert("Start and Destination are the same.");
        return;
    }

    renderStaticMap(); // Reset
    marker.style.display = "none";

    // Use BFS logic to find the route
    const path = findPath(start, end);

    if (!path) {
        statusLabel.innerText = "No route logic found between these cities.";
        return;
    }

    statusLabel.innerHTML = `Route: <b>${path.join(" &rarr; ")}</b>`;
    animatePath(path);
}

function animatePath(pathCities) {
    let currentStep = 0;

    const startCoords = cityCoords[pathCities[0]];
    marker.style.left = startCoords.x + "%";
    marker.style.top = startCoords.y + "%";
    marker.style.display = "block";

    document.getElementById(`node-${pathCities[0]}`).classList.add("active-node");

    function nextStep() {
        if (currentStep >= pathCities.length - 1) {
            statusLabel.innerHTML += " <span style='color:green; font-weight:bold;'> (ARRIVED AT DESTINATION)</span>";
            return;
        }

        const c1 = cityCoords[pathCities[currentStep]];
        const c2 = cityCoords[pathCities[currentStep + 1]];

        drawSVGLine(c1, c2, "active-route-path");

        statusLabel.innerHTML = `Transit: ${pathCities[currentStep]} &rarr; ${pathCities[currentStep + 1]}`;

        const duration = 1500;
        marker.style.transition = `top ${duration}ms linear, left ${duration}ms linear`;

        setTimeout(() => {
            marker.style.left = c2.x + "%";
            marker.style.top = c2.y + "%";
        }, 50);

        setTimeout(() => {
            document.getElementById(`node-${pathCities[currentStep + 1]}`).classList.add("active-node");
            currentStep++;
            nextStep();
        }, duration + 100);
    }

    nextStep();
}

window.onload = initAdvancedMap;
window.onresize = renderStaticMap;
