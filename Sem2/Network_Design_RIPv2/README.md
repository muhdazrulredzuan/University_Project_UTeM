# 🌐 Network Planning Design using RIPv2

This project demonstrates the **planning, configuration, and simulation of a small-scale network** using the **Routing Information Protocol Version 2 (RIPv2)**.  
It focuses on **dynamic routing**, **IP addressing**, and **inter-router communication** using **Cisco Packet Tracer**.

---

## 🖼️ Network Topology
<p align="center">
  <img src="Network_Planning_Design._RIPv2.png" alt="Network Planning Design RIPv2 Topology" width="550"/>
</p>


---

## 📄 Project Report
📘 [View Report (PDF)](Network_Planning_Design_RIPv2_Report.pdf)

---

## 💾 Project File
[⬇️ **Download Packet Tracer File (.pkt)**](Network_Planning_Design_RIPv2.pkt)

---

## ⚙️ Project Description

The **Network Planning Design using RIPv2** project involves building a simulated internetwork between multiple routers and end devices.  
The design implements **RIPv2 dynamic routing** to enable **automatic route exchange** between networks.

### 🧩 Network Overview:
- Multiple routers connected via **Serial interfaces**
- End devices connected through **FastEthernet ports**
- **RIPv2** used for dynamic route propagation
- **Subnetting** applied for efficient IP allocation
- **Static hostname**, **passwords**, and **interface configuration** implemented

---

## 🧠 Key RIPv2 Configuration Commands

```bash
Router> enable
Router# configure terminal
Router(config)# router rip
Router(config-router)# version 2
Router(config-router)# network 10.0.0.0
Router(config-router)# network 202.185.2.0
Router(config-router)# no auto-summary
Router(config-router)# exit
Router# copy running-config startup-config
