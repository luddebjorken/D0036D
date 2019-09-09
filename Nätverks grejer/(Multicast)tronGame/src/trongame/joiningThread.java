/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package trongame;

import java.awt.Color;
import java.io.IOException;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author ludfra-7-local
 */
public class joiningThread extends Thread {
    connectWindow window;
    public joiningThread(connectWindow window){
        this.window = window;
    }
    @Override
    public void run(){
        while(true){
            try {
                String msg = window.in.readUTF();
                System.out.println("Message: " + msg);
                
                if(msg.charAt(0)=='0'){ //On server response
                    //ID
                    int index = msg.indexOf('|');
                    window.id = Integer.parseInt((msg.substring(index+1, index+2)));
                    
                    window.tronText.setForeground(playerColor(window.id));
                    
                    if(window.id==0){ 
                        window.joinButton.setText("Play");
                    }else{
                        window.joinButton.setEnabled(false);
                    }

                    index = msg.indexOf('|',index+1);
                    window.playerCount = Integer.parseInt((msg.substring(index+1, index+2)));

                    System.out.println("ID: " + window.id);
                    System.out.println("Players: " + window.playerCount);

                    window.names = new String[window.playerCount];
                    String info = "";
                    
                    index = msg.indexOf('|',index+1);
                    for(int i = 0; i < window.playerCount; i++){
                        System.out.println("DEBUG: " + msg.indexOf(',',index));
                        window.names[i] = msg.substring(index+1, msg.indexOf(',',index+1));
                        info += "Player " + (i+1) + ": " +window.names[i] + "\n";
                        index = msg.indexOf(',',index+1);
                    }
                    window.setInfo(info);

                }else if(msg.charAt(0)=='1'){
                    window.startGame();
                    System.out.println("Exit");
                    break;
                }
                
            } catch (IOException ex) {
                Logger.getLogger(joiningThread.class.getName()).log(Level.SEVERE, null, ex);
            }
            try {
                Thread.sleep(50);
            } catch (InterruptedException ex) {
                Logger.getLogger(joiningThread.class.getName()).log(Level.SEVERE, null, ex);
            }
        }
    }
        private Color playerColor(int p){
        switch(p){
            case 0:
                return new Color(249, 158, 74);
            case 1:
                return new Color(0,125,125);
            case 2:
                return new Color(125,0,0);
            case 3:
                return new Color(0,125,0);
            default:
                return new Color(187,187,187);
        }
    }
}
