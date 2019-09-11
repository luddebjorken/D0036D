/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package tronserver;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.MulticastSocket;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author ludfra-7-local
 */
public class multicastThread extends Thread {
    String name;
    InetAddress address;
    int port;
    
    public multicastThread(String name, InetAddress address, int port){
        this.name = name;
        this.address = address;
        this.port = port;
    }
    
    @Override
    public void run(){
        MulticastSocket socket = null;
        byte[] buf = new byte[256];
        byte[] responseMsg = ("SERVICE RESPONSE tronJavaGame " + name + " " + address + " " + port).getBytes();
        DatagramPacket packet = new DatagramPacket(buf, buf.length);
        boolean running = true;
        try {
            socket = new MulticastSocket(1900);
            InetAddress group = InetAddress.getByName("239.255.255.250");
            socket.joinGroup(group);
            System.out.println("");
            
        } catch (IOException ex) {
            Logger.getLogger(multicastThread.class.getName()).log(Level.SEVERE, null, ex);
        }
        while(running){
            try {
                socket.receive(packet);
                String msg = new String(packet.getData(), 0, packet.getLength());
                System.out.println(packet.getAddress().toString());
                if(msg.equals("SERVICE REQUEST tronJavaGame")){
                    Thread.sleep(100);
                    DatagramSocket out = new DatagramSocket();
                    
                    DatagramPacket response = new DatagramPacket(responseMsg, responseMsg.length, packet.getAddress(), 1901);
                    out.send(response);
                    System.out.println("Response sent to " + packet.getAddress());
                    
                }
            } catch (IOException ex) {
                Logger.getLogger(multicastThread.class.getName()).log(Level.SEVERE, null, ex);
            } catch (InterruptedException ex) {
                Logger.getLogger(multicastThread.class.getName()).log(Level.SEVERE, null, ex);
            }
        }
    }
}
