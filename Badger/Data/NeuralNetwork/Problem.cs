/*
	SimionZoo: A framework for online model-free Reinforcement Learning on continuous
	control problems

	Copyright (c) 2016 SimionSoft. https://github.com/simionsoft

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

using Badger.Data.NeuralNetwork.Links;
using Badger.Data.NeuralNetwork.Parameters;
using System;
using System.Collections.Generic;
using System.Runtime.Serialization;
using System.Xml;
using Xml.Serialization;
using Badger.Data.NeuralNetwork.Optimizers;

namespace Badger.Data.NeuralNetwork
{
    public class Problem : IDeserializationCallback
    {
        public Problem()
        {
            NetworkArchitecture = new NetworkArchitecture(this);
            OptimizerSetting = new OptimizerSetting(OptimizerType.SGD);
        }

        public List<InputData> Inputs { get; } = new List<InputData>();

        public OptimizerSetting OptimizerSetting { get; set; }

        public OutputConfiguration Output { get; set; } = new OutputConfiguration();

        public NetworkArchitecture NetworkArchitecture { get; set; }

        public void Export()
        {
            var serializer = new XmlCallbackSerializer(typeof(Problem), new Type[] {
                    typeof(NetworkArchitecture),
                    typeof(ActivationLayer), typeof(Convolution1DLayer), typeof(Convolution2DLayer), typeof(Convolution3DLayer), typeof(DenseLayer),
                    typeof(DropoutLayer), typeof(FlattenLayer), typeof(ReshapeLayer), typeof(MergeLayer),
                    typeof(BatchNormalizationLayer), typeof(LinearTransformationLayer),
                    typeof(ActivationFunctionParameter), typeof(DoubleParameter), typeof(IntParameter),
                    typeof(IntTuple2DParameter),  typeof(IntTuple3DParameter),  typeof(IntTuple4DParameter)
                });
            using (var sw = new System.IO.StreamWriter("network.xml"))
            {
                using (XmlWriter writer = XmlWriter.Create(sw, new XmlWriterSettings() { OmitXmlDeclaration = true, Indent = true }))
                {
                    serializer.Serialize(writer, this);
                }
            }
        }

        public string ExportToString()
        {
            var serializer = new XmlCallbackSerializer(typeof(Problem), new Type[] {
                    typeof(NetworkArchitecture),
                    typeof(ActivationLayer), typeof(Convolution1DLayer), typeof(Convolution2DLayer),
                    typeof(Convolution3DLayer), typeof(DenseLayer),
                    typeof(BatchNormalizationLayer), typeof(LinearTransformationLayer),
                    typeof(DropoutLayer), typeof(FlattenLayer), typeof(ReshapeLayer), typeof(MergeLayer),
                    typeof(ActivationFunctionParameter), typeof(DoubleParameter), typeof(IntParameter),
                    typeof(IntTuple2DParameter),  typeof(IntTuple3DParameter),  typeof(IntTuple4DParameter)
                });
            using (var sw = new System.IO.StringWriter())
            {
                using (XmlWriter writer = XmlWriter.Create(sw, new XmlWriterSettings() { OmitXmlDeclaration = true, Indent = true }))
                {
                    serializer.Serialize(writer, this);
                }

                return sw.ToString();
            }
        }
        public static Problem Import(List<InputData> inputs)
        {
            var serializer = new XmlCallbackSerializer(typeof(Problem), new Type[] {
                    typeof(NetworkArchitecture),
                    typeof(ActivationLayer), typeof(Convolution1DLayer), typeof(Convolution2DLayer),
                    typeof(Convolution3DLayer), typeof(DenseLayer),
                    typeof(DropoutLayer), typeof(FlattenLayer), typeof(ReshapeLayer), typeof(MergeLayer),
                    typeof(BatchNormalizationLayer), typeof(LinearTransformationLayer),
                    typeof(ActivationFunctionParameter), typeof(DoubleParameter), typeof(IntParameter),
                    typeof(IntTuple2DParameter),  typeof(IntTuple3DParameter),  typeof(IntTuple4DParameter)
                });
            using (var sr = new System.IO.StreamReader("network.xml"))
            {
                using (XmlReader reader = XmlReader.Create(sr))
                {
                    Problem result = serializer.Deserialize(reader) as Problem;

                    foreach (var input in result.Inputs)
                    {
                        foreach (var oldInput in inputs)
                        {
                            if (input.ID.Equals(oldInput.ID))
                            {
                                input.Description = oldInput.Description;
                                inputs.Remove(oldInput);
                                break;
                            }
                        }
                    }

                    foreach (var item in inputs)
                        result.Inputs.Add(item);

                    return result;
                }
            }
        }

        public static Problem Import(string content, List<InputData> inputs)
        {
            var serializer = new XmlCallbackSerializer(typeof(Problem), new Type[] {
                    typeof(NetworkArchitecture),
                    typeof(ActivationLayer), typeof(Convolution1DLayer), typeof(Convolution2DLayer), typeof(Convolution3DLayer), typeof(DenseLayer),
                    typeof(DropoutLayer), typeof(FlattenLayer), typeof(ReshapeLayer), typeof(MergeLayer),
                    typeof(BatchNormalizationLayer), typeof(LinearTransformationLayer),
                    typeof(ActivationFunctionParameter), typeof(DoubleParameter), typeof(IntParameter),
                    typeof(IntTuple2DParameter),  typeof(IntTuple3DParameter),  typeof(IntTuple4DParameter)
                });
            using (var sr = new System.IO.StringReader(content))
            {
                using (XmlReader reader = XmlReader.Create(sr))
                {
                    Problem result;
                    try
                    {
                        result = serializer.Deserialize(reader) as Problem;
                    }
                    catch
                    {
                        result = new Problem();

                    }

                    foreach (var input in result.Inputs)
                    {
                        foreach (var oldInput in inputs)
                        {
                            if (input.ID.Equals(oldInput.ID))
                            {
                                input.Description = oldInput.Description;
                                inputs.Remove(oldInput);
                                break;
                            }
                        }
                    }

                    foreach (var item in inputs)
                        result.Inputs.Add(item);

                    return result;
                }
            }
        }

        public void OnDeserialization(object sender)
        {
            NetworkArchitecture.Problem = this;

            //fix the Output node again
            if (Output != null && Output.LinkConnection != null)
            {
                foreach (var chain in NetworkArchitecture.Chains)
                {
                    foreach (var link in chain.ChainLinks)
                    {
                        if (link.ID.Equals(Output.LinkConnection.TargetID))
                        {
                            Output.LinkConnection.Target = link;
                            break;
                        }
                    }
                }
            }
        }
    }
}
