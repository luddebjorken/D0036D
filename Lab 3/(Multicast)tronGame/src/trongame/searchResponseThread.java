/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package trongame;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.SocketException;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author ludfra-7-local
 */
public class searchResponseThread extends Thread {
    public boolean running = true;
    connectWindow window;
    
    public searchResponseThread(connectWindow window){
        this.window = window;
    }
    
    @Override
    public void run(){
        
        try {
            window.serverList = new serverObject[10];
            DatagramSocket in = new DatagramSocket(1901);
            byte[] buf = new byte[256];
            DatagramPacket packet = new DatagramPacket(buf, buf.length);
            window.serverSelectorBox.removeAllItems();
            while(running){
                try {
                    in.receive(packet);
                    String msg = new String(packet.getData(), 0, packet.getLength());
                    if(msg.startsWith("SERVICE RESPONSE tronJavaGame")){
                        System.out.println("Server reply: " + msg);
                        String[] data = msg.substring(msg.indexOf("tronJavaGame")+13).split(" ");
                        window.serverList[window.serverCount] = new serverObject(data[0], data[1], data[2]);
                        window.serverSelectorBox.addItem(window.serverList[window.serverCount].name);
                        break;
                    }else{
                        System.out.println("Invalid message: " + msg);
                    }
                    Thread.sleep(50);
                    
                } catch (IOException ex) {
                    Logger.getLogger(searchResponseThread.class.getName()).log(Level.SEVERE, null, ex);
                } catch (InterruptedException ex) {
                    Logger.getLogger(searchResponseThread.class.getName()).log(Level.SEVERE, null, ex);
                }
            }
            
        in.close();
        System.out.println("Socket closed");
        } catch (SocketException ex) {
            Logger.getLogger(searchResponseThread.class.getName()).log(Level.SEVERE, null, ex);
        }
            
    }
}

