// Core Logic using LocalStorage simulation

// Initial data if storage is empty
const initialData = {
    "SW1001": { id: "SW1001", sender: "Karachi", receiver: "Lahore", weight: 5, priority: 1, status: "In Transit", date: "Jan 15, 2025" },
    "SW1002": { id: "SW1002", sender: "Hyderabad", receiver: "Islamabad", weight: 2, priority: 2, status: "Delivered", date: "Jan 10, 2025" },
    "SW1003": { id: "SW1003", sender: "Multan", receiver: "Sukkur", weight: 12, priority: 3, status: "Booked", date: "Jan 20, 2025" },
    "SW1004": { id: "SW1004", sender: "Islamabad", receiver: "Karachi", weight: 1, priority: 1, status: "Out for Delivery", date: "Jan 12, 2025" }
};

// History Stack for Undo
let historyStack = [];

// Helper: Get formatted date
function getCurrentDate() {
    const d = new Date();
    return d.toLocaleDateString('en-US', { month: 'short', day: 'numeric', year: 'numeric' });
}

// Data Access Layer
const SwiftExApp = {
    getAll: function () {
        const stored = localStorage.getItem('swiftExData');
        return stored ? JSON.parse(stored) : initialData;
    },

    saveAll: function (data) {
        localStorage.setItem('swiftExData', JSON.stringify(data));
    },

    // 1. Add Parcel
    addParcel: function (parcel) {
        const data = this.getAll();
        if (data[parcel.id]) {
            return false;
        }
        data[parcel.id] = parcel;
        this.saveAll(data);
        this.pushHistory("ADD", parcel.id, null);
        return true;
    },

    getParcel: function (id) {
        const data = this.getAll();
        return data[id] || null;
    },

    // 7. Cancel Parcel
    cancelParcel: function (id) {
        const data = this.getAll();
        if (data[id]) {
            const oldStatus = data[id].status;
            data[id].status = "Cancelled";
            this.saveAll(data);
            this.pushHistory("STATUS", id, oldStatus);
            return true;
        }
        return false;
    },

    // 2. Dispatch Warehouse (Process Next)
    // Moves the first "Booked" parcel to "In Transit"
    processNext: function () {
        const data = this.getAll();
        const keys = Object.keys(data);

        for (let key of keys) {
            if (data[key].status === "Booked") {
                data[key].status = "In Transit";
                this.saveAll(data);
                this.pushHistory("STATUS", key, "Booked");
                return data[key]; // Return processed parcel
            }
        }
        return null; // Nothing to process
    },

    // 8. Undo Last Action
    pushHistory: function (type, id, oldVal) {
        historyStack.push({ type: type, id: id, oldVal: oldVal });
        // Simulating session history (not persistent across reload for simplicity)
    },

    undoLast: function () {
        if (historyStack.length === 0) return null;

        const action = historyStack.pop();
        const data = this.getAll();

        if (action.type === "STATUS") {
            // Revert status
            if (data[action.id]) {
                data[action.id].status = action.oldVal;
                this.saveAll(data);
                return `Reverted ${action.id} to ${action.oldVal}`;
            }
        } else if (action.type === "ADD") {
            // Remove added parcel
            delete data[action.id];
            this.saveAll(data);
            return `Removed created parcel ${action.id}`;
        }
        return "Undo failed";
    },

    resetData: function () {
        localStorage.removeItem('swiftExData');
        location.reload();
    }
};

// --- Page Specific Logic --- //

// 1. Dashboard Stats & Functions
function loadDashboardStats() {
    const data = SwiftExApp.getAll();
    const total = Object.keys(data).length;
    let delivered = 0;
    let transit = 0;
    let booked = 0;

    Object.values(data).forEach(p => {
        if (p.status === "Delivered") delivered++;
        if (p.status === "Booked") booked++;
        if (p.status === "In Transit" || p.status === "Out for Delivery") transit++;
    });

    if (document.getElementById('statTotal')) document.getElementById('statTotal').innerText = total;
    if (document.getElementById('statDelivered')) document.getElementById('statDelivered').innerText = delivered;
    if (document.getElementById('statTransit')) document.getElementById('statTransit').innerText = transit;

    // Check if there are items to dispatch
    const dispatchBtn = document.getElementById('btnDispatch');
    if (dispatchBtn) {
        dispatchBtn.innerText = `Dispatch Next (${booked} Waiting)`;
        dispatchBtn.disabled = booked === 0;
    }
}

// 2. Dispatch Handler
function handleDispatch() {
    const result = SwiftExApp.processNext();
    if (result) {
        alert(`DISPATCHED: Parcel ${result.id} is now In Transit to ${result.receiver}!`);
        loadDashboardStats(); // Refresh stats
    } else {
        alert("Warehouse Empty! No booked parcels to dispatch.");
    }
}

// 8. Undo Handler
function handleUndo() {
    const msg = SwiftExApp.undoLast();
    if (msg) {
        alert("UNDO: " + msg);
        loadDashboardStats();
    } else {
        alert("Nothing to undo!");
    }
}

// 3. Add Parcel Form
function handleAddParcel(e) {
    if (e) e.preventDefault();

    const id = document.getElementById('pId').value.toUpperCase();
    const sender = document.getElementById('pSender').value;
    const receiver = document.getElementById('pReceiver').value;
    const weight = document.getElementById('pWeight').value;
    const priority = document.getElementById('pPriority').value;

    if (!id || !sender || !receiver) {
        alert("Please fill all fields");
        return;
    }

    const newParcel = {
        id: id,
        sender: sender,
        receiver: receiver,
        weight: weight,
        priority: priority,
        status: "Booked",
        date: getCurrentDate()
    };

    const success = SwiftExApp.addParcel(newParcel);
    if (success) {
        alert("Parcel " + id + " Added Successfully!");
        window.location.href = "inventory.html";
    } else {
        alert("Error: Parcel ID " + id + " already exists!");
    }
}

// 4. Inventory List
function loadInventory() {
    const data = SwiftExApp.getAll();
    const tbody = document.getElementById('inventoryBody');
    if (!tbody) return;

    tbody.innerHTML = "";

    Object.values(data).forEach(p => {
        let statusClass = "status-booked";
        if (p.status.includes("Transit")) statusClass = "status-transit";
        if (p.status.includes("Delivery")) statusClass = "status-delivery";
        if (p.status === "Delivered") statusClass = "status-delivered";
        if (p.status === "Cancelled") statusClass = "status-cancelled";

        const row = `
            <tr>
                <td><b style="color:var(--primary-color)">${p.id}</b></td>
                <td>${p.sender}</td>
                <td>${p.receiver}</td>
                <td>${p.date}</td>
                <td><span class="status-badge ${statusClass}">${p.status}</span></td>
                <td>
                    <button class="btn btn-danger" style="padding: 0.3rem 0.6rem; font-size: 0.8rem;" onclick="cancel('${p.id}')">X</button>
                    <a href="track_parcel.html?id=${p.id}" style="margin-left:5px; color: var(--primary-color); text-decoration:none;">Track</a>
                </td>
            </tr>
        `;
        tbody.innerHTML += row;
    });
}

function cancel(id) {
    if (confirm('Cancel parcel ' + id + '?')) {
        SwiftExApp.cancelParcel(id);
        loadInventory();
    }
}

// 5 & 6. Tracking Page Logic
function initTrackingPage() {
    // Check URL params
    const urlParams = new URLSearchParams(window.location.search);
    const idFromUrl = urlParams.get('id');
    if (idFromUrl) {
        document.getElementById('trackingIdInput').value = idFromUrl;
        trackParcelUserAction();
    }
}

function trackParcelUserAction() {
    const id = document.getElementById("trackingIdInput").value.trim().toUpperCase();
    trackParcelLogic(id);
}

function trackParcelLogic(id) {
    const p = SwiftExApp.getParcel(id);
    const resultBox = document.getElementById("resultBox");
    const errorMsg = document.getElementById("errorMsg");

    if (p) {
        document.getElementById("displayId").innerText = p.id;
        document.getElementById("displaySender").innerText = p.sender;
        document.getElementById("displayReceiver").innerText = p.receiver;
        document.getElementById("displayDate").innerText = p.date;

        const badge = document.getElementById("displayStatus");
        badge.innerText = p.status;

        // Reset classes
        let statusClass = "status-booked";
        if (p.status.includes("Transit")) statusClass = "status-transit";
        if (p.status.includes("Delivery")) statusClass = "status-delivery";
        if (p.status === "Delivered") statusClass = "status-delivered";
        if (p.status === "Cancelled") statusClass = "status-cancelled";
        badge.className = "status-badge " + statusClass;

        resultBox.style.display = "block";
        errorMsg.style.display = "none";
    } else {
        resultBox.style.display = "none";
        errorMsg.style.display = "block";
    }
}
