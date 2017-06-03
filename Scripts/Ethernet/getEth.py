import os

print "HWAPI::Scripts::getEth"
os.system("ipconfig /all > eth.txt")
f = open("eth.txt", "r")
content = f.readlines();
f.close()


iters = []
for element in range(len(content)) :
    if content[element].find("Ethernet adapter Local Area Connection") != -1:
        iters.append(element)

iterends = []
for iter in iters:
    for element in range(iter, len(content)):
        if element+1 < len(content) and content[element + 1].find("NetBIOS") != -1:
            iterends.append(element+1)

print  "HWAPI::Scripts::getEth: Number of interfaces: ", len(iters)
interfaces = []
for iter in range(len(iters)):
    if iterends[iter] !=0 :
        interfaces.append(content[iters[iter]:iterends[iter]])

name=interfaces[0][0].replace(":", "").replace("\n", "")
ip=""
gw=""
phisicalAddress=""
mask=""
dhcp=""
for element in interfaces[0]:

    if element.find("IPv4") != -1:
        splited = element.split(":")
        tmp = splited[1].find("(")
        if tmp != -1:
            ip = splited[1][:tmp]
    elif element.find("Physical Address") != -1:
        splited = element.split(":")
        phisicalAddress = splited[1].replace('\n', "")
    elif element.find("DHCP Enabled") != -1:
        splited = element.split(":")
        dhcp = splited[1].replace('\n', "")
    elif element.find("Subnet Mask") != -1:
        splited = element.split(":")
        mask = splited[1].replace('\n', "")

print "HWAPI::Scripts::getEth: NAME: ", name
print "HWAPI::Scripts::getEth: IP: ", ip
print "HWAPI::Scripts::getEth: MASK: ", mask
print "HWAPI::Scripts::getEth: DHCP: ", dhcp
f = open("D:\\private\\OSCAR\\New_Architecture_OSCAR\\OSCAR\\Scripts\\Ethernet\\ETH_CONFIG.txt", "w")
message = "NAME:"+name.replace(" ", "")+";"+"IP:"+ip.replace(" ", "")+";"+"MASK:"+mask.replace(" ", "")+";"+"DHCP:"+dhcp.replace(" ", "")+";"+"GW:"
f.write(message.replace(" ", "_"))
f.close()
iters = []
for element in range(len(content)) :
    if content[element].find("Wireless LAN adapter Wireless Network Connection") != -1:
        iters.append(element)

iterends = []
for iter in iters:
    for element in range(iter, len(content)):
        if element+1 < len(content) and content[element + 1].find("NetBIOS") != -1:
            iterends.append(element+1)

print  "HWAPI::Scripts::getEth: Number of interfaces: ", len(iters)
interfaces = []
for iter in range(len(iters)):
    if iterends[iter] !=0 :
        interfaces.append(content[iters[iter]:iterends[iter]])

name=interfaces[0][0].replace(":", "").replace("\n", "")

ip=""
gw=""
phisicalAddress=""
mask=""
dhcp=""
for element in interfaces[0]:

    if element.find("IPv4") != -1:
        splited = element.split(":")
        tmp = splited[1].find("(")
        if tmp != -1:
            ip = splited[1][:tmp]
    elif element.find("Physical Address") != -1:
        splited = element.split(":")
        phisicalAddress = splited[1].replace('\n', "")
    elif element.find("DHCP Enabled") != -1:
        splited = element.split(":")
        dhcp = splited[1].replace('\n', "")
    elif element.find("Subnet Mask") != -1:
        splited = element.split(":")
        mask = splited[1].replace('\n', "")
    elif element.find("Default Gateway") != -1:
        splited = element.split(":")
        gw = splited[1].replace('\n', "")

print "HWAPI::Scripts::getEth: NAME: ", name
print "HWAPI::Scripts::getEth: IP: ", ip
print "HWAPI::Scripts::getEth: MASK: ", mask
print "HWAPI::Scripts::getEth: DHCP: ", dhcp
print "HWAPI::Scripts::getEth: GW: ", gw

f = open("D:\\private\\OSCAR\\New_Architecture_OSCAR\\OSCAR\\Scripts\\Ethernet\\ETH_CONFIG.txt", "a")
message = "#NAME:"+name.replace(" ", "")+";"+"IP:"+ip.replace(" ", "")+";"+"MASK:"+mask.replace(" ", "")+";"+"DHCP:"+dhcp.replace(" ", "")+";"+"GW:"+gw.replace(" ", "")
f.write(message.replace(" ", "_"))
f.close()