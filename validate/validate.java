package validate;

import org.jlab.groot.data.H1F;
import org.jlab.groot.ui.TCanvas;
import org.jlab.jnp.hipo4.data.Bank;
import org.jlab.jnp.hipo4.data.Event;
import org.jlab.jnp.hipo4.data.SchemaFactory;
import org.jlab.jnp.hipo4.io.HipoReader;

import org.jlab.jroot.ROOTFile;

public class validate {
    public static void main(final String[] args) {
        String Filename = "";
        if (args.length > 0) {
            Filename = args[0];
        } else {
            Filename = "/Users/tylern/Data/hipo/rg-a/skim4_005038.hipo";
        }

        final TCanvas can = new TCanvas("DeltaT Proton", 1600, 900);
        can.divide(2, 2);
        can.getCanvas().initTimer(1000);
        can.cd(0);
        final H1F h1 = new H1F("h1", "Hipo PIDs", 8000, -4000, 4000);
        can.draw(h1);
        can.cd(1);
        final H1F h2 = new H1F("h2", "Electron Px", 500, -5, 5);
        can.draw(h2);
        can.cd(2);
        final H1F h3 = new H1F("h3", "FTOF 1B Energy", 250, 0, 50);
        can.draw(h3);
        final HipoReader reader = new HipoReader();
        reader.open(Filename);

        final SchemaFactory factory = reader.getSchemaFactory();
        final Bank rec_Particle = new Bank(factory.getSchema("REC::Particle"));
        final Bank rec_Scintillator = new Bank(factory.getSchema("REC::Scintillator"));

        final Event event = new Event();

        while (reader.hasNext()) {
            reader.nextEvent(event);
            event.read(rec_Particle);
            event.read(rec_Scintillator);

            if (rec_Particle.getRows() > 0) { // cut # 1 (gpart > 0).

                // Then to fill
                // If particle id at 0 is 11 fill in the histogram h2 with px.
                if (rec_Particle.getInt("pid", 0) == 11) {
                    h2.fill(rec_Particle.getFloat("px", 0));
                }

                // Then looping over particles 1 to the number of particles :
                for (int part = 1; part < rec_Particle.getRows(); part++) {
                    // Fill the PID of every particle into h1
                    h1.fill(rec_Particle.getInt("pid", part));

                    // Fill the energy component of FTOF 1b into h3.
                    for (int k = 0; k < rec_Scintillator.getRows(); k++) {
                        final int pindex = rec_Scintillator.getShort("pindex", k);
                        final int detector = rec_Scintillator.getInt("detector", k);
                        final int layer = rec_Scintillator.getInt("layer", k);
                        if (pindex == part && detector == 12 && layer == 2)
                            h3.fill(rec_Scintillator.getFloat("energy", k));
                    }
                }
            }

        }

        can.save("validate.png");
        System.exit(0);
    }
}