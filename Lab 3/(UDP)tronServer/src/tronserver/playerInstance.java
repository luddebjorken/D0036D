/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package tronserver;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author ludfra-7-local
 */
public class playerInstance extends Thread {
    TronServer serverInstance;
    gameInstance game;
    
    //TCP stuff
    ServerSocket serverSocket;
    Socket client;
    DataInputStream in;
    DataOutputStream out;
    
    //UDP stuff
    DatagramSocket UDPsocket;
    InetAddress dest;
    int port = 25565;
    
    int state = 0; //0=menu, 1=alive, 2=dead
    int id;
    String name;
    

    playerInstance(ServerSocket ss, int id, TronServer ts) throws IOException {
        this.id = id;
        this.serverInstance = ts;
        this.serverSocket = ss;
        
        System.out.println("Thread #" + id + " started!");
        
        
    }
    
    @Override
    public void run(){
        try{
            client = serverSocket.accept();
            System.out.println("Client accepted @ " + client.getInetAddress().getHostAddress() + ":" + client.getPort());
            in = new DataInputStream(client.getInputStream());
            out = new DataOutputStream(client.getOutputStream());

            //Saves name
            while(true){
                String msg = in.readUTF();
                if(msg.startsWith("0")){
                    name = msg.substring(2);
                    serverInstance.names[id] = name;
                    break;
                }
            }
            port += id+1; //To run locally
            System.out.println("UDP opened on port " + port);
            UDPsocket = new DatagramSocket();
            System.out.println("Name: " + name + ":" + id);

            for (int i = 0; i <= serverInstance.players; i++) {
                serverInstance.pInstance[i].newPlayer();
            }
            dest = client.getInetAddress();
        }catch(IOException ex){
            Logger.getLogger(playerInstance.class.getName()).log(Level.SEVERE, null, ex);
        }
        
        try {
            Thread.sleep(500);
        } catch (InterruptedException ex) {
            Logger.getLogger(playerInstance.class.getName()).log(Level.SEVERE, null, ex);
        }
        serverInstance.waiting = false;
        waitForStart();
    }
    
    public void waitForStart(){
        System.out.println("Thread waiting...");
        while(true){
            try {
                String msg = in.readUTF();
                if(msg.startsWith("1")){
                    if(serverInstance.players>0){ //Failsafe
                        serverInstance.startRound();
                        gameInput();
                        break;
                    }
                }else if(msg.startsWith("2")){
                    gameInput();
                    break;
                }

            } catch (IOException ex) {
                Logger.getLogger(playerInstance.class.getName()).log(Level.SEVERE, null, ex);
            }
            try {
                Thread.sleep(500);
            } catch (InterruptedException ex) {
                Logger.getLogger(playerInstance.class.getName()).log(Level.SEVERE, null, ex);
            }
        }
    }
    
    public void gameInput(){
        while(state==1){
            try {
                String msg = in.readUTF();
                System.out.println("In: " + msg);
                switch(msg.charAt(0)){
                    case '2':
                        System.out.println("2ASD");
                        if(msg.substring(2,3).equals(id)) {
                            System.out.println("Message from wrong client " + msg.charAt(2)+":"+id);
                            break;
                        } //Cancels if from wrong client (failsafe)
                        msg = msg.substring(msg.indexOf("|")+1);
                        game.vel[id] = new int[]{Integer.parseInt(msg.substring(msg.indexOf('|')+1,msg.indexOf(','))), Integer.parseInt(msg.substring(msg.indexOf(',')+1,msg.length()))};
                        System.out.println(game.vel[id][0] + "===" + game.vel[id][1]);
                        break;
                    case '3':
                        //Send field
                        break;
                }
            } catch (IOException ex) {
                Logger.getLogger(playerInstance.class.getName()).log(Level.SEVERE, null, ex);
            }
        }
    }
    
    //Method for informing clients of new players. Called whenever new player joins
    public void newPlayer() throws IOException{
        
        //Constructs message string
        String names = "";
        for(int i = 0; i <= serverInstance.players; i++){
            names+=serverInstance.names[i]+",";
        }
        
        //Sends names of all players
        System.out.println(names);
        System.out.println("PLayer" + id);
        out.writeUTF("0|"+ id + "|" + (serverInstance.players+1) +"|"+names);
        System.out.println("0|"+ id + "|" +(serverInstance.players+1)+"|"+names);
        out.flush();
        
    }
    
    public void sendPos(int tick){
        String velocities = "";
        for (int i = 0; i < serverInstance.players; i++) {
            checkDead();
            if(game.pInstance[i].state != 2) velocities+= game.pos[i][0] + "," + game.pos[i][1] + ";" ;
            else velocities+= -10 + "," + -10 + ";" ;
        }
        
        byte[] buf = (1 + "|" + tick + "|" + serverInstance.players + "|" + velocities).getBytes();
        DatagramPacket packet = new DatagramPacket(buf, buf.length, dest, port);
        
        try {
            UDPsocket.send(packet);
        } catch (IOException ex) {
            Logger.getLogger(playerInstance.class.getName()).log(Level.SEVERE, null, ex);
        }
    }
    public void sendWin(String name){
        try {
            byte[] buf = ("3|"+name).getBytes();
            DatagramPacket packet = new DatagramPacket(buf, buf.length, dest, port);
            UDPsocket.send(packet);
            client.close();
        } catch (IOException ex) {
            Logger.getLogger(playerInstance.class.getName()).log(Level.SEVERE, null, ex);
        }
    }
    public void sendStart(){
        try {
            out.writeUTF("1|Start");
        } catch (IOException ex) {
            Logger.getLogger(playerInstance.class.getName()).log(Level.SEVERE, null, ex);
        }
    }
    
    void checkDead(){
        if(state!=2){
            if(game.pos[id][0]>31 ||game.pos[id][0]<0 ||game.pos[id][1]>31 ||game.pos[id][1]<0 ){
                System.out.println("Player " + id + " has perished!!");
                state = 2;
            }else if(game.field[game.pos[id][0]][game.pos[id][1]]!=-1){
                System.out.println("Player " + id + " has perished!!");
                state = 2;
            }
        }
        
    }
}
